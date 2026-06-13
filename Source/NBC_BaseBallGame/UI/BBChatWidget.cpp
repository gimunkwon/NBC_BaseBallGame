#include "BBChatWidget.h"

#include "BBChatMessageEntry.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "NBC_BaseBallGame/GameState/BBGameState.h"
#include "NBC_BaseBallGame/PlayerController/BBPlayerController.h"

void UBBChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_SendChat->OnClicked.AddDynamic(this, &UBBChatWidget::OnSendButtonClicked);
	ET_InputChat->OnTextCommitted.AddDynamic(this, &UBBChatWidget::OnInputTextCommitted);
	Btn_ReadyORReset->OnClicked.AddDynamic(this, &UBBChatWidget::OnStartGameButtonClicked);
}

// 동적 스폰된 위젯을 스크롤박스에 붙여주는 메서드
void UBBChatWidget::AddMessage(const FString& InMessage)
{
	if (!ChatMessageEntryClass)
	{
		return;
	}
	
	UBBChatMessageEntry* Entry = CreateWidget<UBBChatMessageEntry>(this, ChatMessageEntryClass);
	if (Entry)
	{
		Entry->SetMessage(InMessage);
		ScrollBox_ChatLog->AddChild(Entry);
		ScrollBox_ChatLog->ScrollToEnd();
		UE_LOG(LogTemp,Warning,TEXT("ScrollBox_ChatLog Current Child : %s"),*ScrollBox_ChatLog->GetChildAt(0)->GetName());
	}
}

// 버튼 입력 발생시 PlayerController의 ServerRPC 호출
// EditableTextBox를 비워줌
void UBBChatWidget::OnSendButtonClicked()
{
	const FString Message = ET_InputChat->GetText().ToString();
	if (Message.IsEmpty()) return;
	
	ABBPlayerController* PC = Cast<ABBPlayerController>(GetOwningPlayer());
	if (PC)
	{
		// 모드에 따른 다른 RPC 호출
		if (bIsGuessMode)
		{
			// 정답 채팅
			PC->ServerSubmitGuess(Message);
		}
		else
		{
			// 일반 채팅
			PC->ServerSendChat(Message);
		}
	}
	
	ET_InputChat->SetText(FText::GetEmpty());
}

// Enter 입력 발생시 OnSendButtonClicked 이벤트 발생
// 방법이 2가지 1. 버튼 누르기 2. 엔터 누르기
void UBBChatWidget::OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter || CommitMethod == ETextCommit::OnCleared && !Text.IsEmpty())
	{
		OnSendButtonClicked();
	}
}

// 이 위젯에 포커싱이 되었을때 키 입력 이벤트가 발생했을 경우 일어나는 이벤트 함수
FReply UBBChatWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// Tab키 입력시 토글(정답/일반채팅)
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		bIsGuessMode = !bIsGuessMode;
		
		FString ModeText = bIsGuessMode ? TEXT("[정답]") : TEXT("[일반]");
		if (Text_ModeDisplay)
		{
			Text_ModeDisplay->SetText(FText::FromString(ModeText));
		}
		return FReply::Handled();
	}
	
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

// 시작/재시작 버튼의 활성화 여부
void UBBChatWidget::SetStartButtonVisibility(bool bVisible)
{
	if (Btn_ReadyORReset)
	{
		Btn_ReadyORReset->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

// 시작/재시작 버튼의 활성화 여부
void UBBChatWidget::SetStartButtonEnable(bool bCanEnable)
{
	if (Btn_ReadyORReset)
	{
		Btn_ReadyORReset->SetIsEnabled(bCanEnable);
	}
}

void UBBChatWidget::OnStartGameButtonClicked()
{
	ABBPlayerController* PC = Cast<ABBPlayerController>(GetOwningPlayer());
	if (!PC) return;

	ABBGameState* GS = Cast<ABBGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	if (GS->GamePhase == EBBGamePhase::Waiting)
	{
		PC->ServerSetReady();
		Btn_ReadyORReset->SetIsEnabled(false);
	}
	else
	{
		PC->ServerRequestRematch();
	}
}

// 턴 시작 시 호출 — 모드 전환 + 카운트다운 시작 + 색상 업데이트
// InTurnDuration=0 으로 호출하면 타이머만 정지
void UBBChatWidget::SetGuessMode(bool bGuess, float InTurnDuration)
{
	bIsGuessMode = bGuess;
	FString ModeText = bIsGuessMode ? TEXT("[정답]") : TEXT("[일반]");
	if (Text_ModeDisplay)
	{
		Text_ModeDisplay->SetText(FText::FromString(ModeText));
	}

	GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
	RemainingSeconds = InTurnDuration;
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(FString::FromInt(FMath::CeilToInt(RemainingSeconds))));
	}
	if (InTurnDuration > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(CountdownHandle, this, &UBBChatWidget::OnCountdownTick, 1.f, true);
	}

	UpdateTurnColors(bGuess);
}

void UBBChatWidget::OnCountdownTick()
{
	RemainingSeconds -= 1.f;
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(FString::FromInt(FMath::Max(0, FMath::CeilToInt(RemainingSeconds)))));
	}
	if (RemainingSeconds <= 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
	}
}

void UBBChatWidget::SetRemainChance(int32 InRemainChance)
{
	if (Text_RemainChance)
	{
		Text_RemainChance->SetText(FText::FromString(FString::Printf(TEXT("남은 기회 : %d"),InRemainChance)));
	}
}

void UBBChatWidget::UpdateTurnColors(bool bIsMyTurn)
{
	const FLinearColor Active(1.f, 1.f, 0.f, 1.f);     // 노란색 — 현재 턴
	const FLinearColor Inactive(0.4f, 0.4f, 0.4f, 1.f); // 회색 — 대기 중

	if (Text_MyTurn)    Text_MyTurn->SetColorAndOpacity(bIsMyTurn ? Active : Inactive);
	if (Text_OtherTurn) Text_OtherTurn->SetColorAndOpacity(bIsMyTurn ? Inactive : Active);
}
