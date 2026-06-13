#include "BBPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "NBC_BaseBallGame/GameMode/BBGameMode.h"
#include "NBC_BaseBallGame/UI/BBChatWidget.h"

// void ABBPlayerController::BeginPlay()
// {
// 	Super::BeginPlay();
// }

ABBPlayerController::ABBPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ABBPlayerController::PostNetInit()
{
	Super::PostNetInit();
	
	UE_LOG(LogTemp,Warning,TEXT("PostNetInit called | IsLocal : %d | WidgetClass: %d"),IsLocalController(), Widget_ChatClass != nullptr);
	
	if (IsLocalController() && Widget_ChatClass)
	{
		Widget_ChatInst = CreateWidget<UBBChatWidget>(this, Widget_ChatClass);
		if (Widget_ChatInst)
		{
			Widget_ChatInst->AddToViewport();
		}
	}
}

void ABBPlayerController::ClientReceiveChat_Implementation(const FString& SenderName, const FString& Message)
{
	FString LastString = FString::Printf(TEXT("[%s] : %s"),*SenderName, *Message);
	// UKismetSystemLibrary::PrintString(this, LastString, true, true, FLinearColor::Green, 5.f);
	
	if (Widget_ChatInst)
	{
		Widget_ChatInst->AddMessage(LastString);
	}
}

void ABBPlayerController::ServerSendChat_Implementation(const FString& Message)
{
	ABBGameMode* GameMode = Cast<ABBGameMode>(GetWorld()->GetAuthGameMode());
	
	if (GameMode)
	{
		FString SenderName = PlayerState ? PlayerState->GetPlayerName() : TEXT("Unknown");
		GameMode->BroadCastChat(SenderName, Message);
	}
}

void ABBPlayerController::ServerSubmitGuess_Implementation(const FString& GuessString)
{
	ABBGameMode* GameMode = Cast<ABBGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		// FString SenderName = PlayerState ? PlayerState->GetPlayerName() : TEXT("Unknown");
		GameMode->ProcessGuess(GuessString, this);
	}
}

// 승리/패배 게임 로직이 분기에 도달시 호출된 ClientRPC
void ABBPlayerController::ClientOnGameResult_Implementation(bool bWin)
{
	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("TVScoreBoard"),FoundActor);
	
	if (FoundActor.Num() > 0)
	{
		UWidgetComponent* WidgetComp = FoundActor[0]->FindComponentByClass<UWidgetComponent>();
		if (WidgetComp)
		{
			// 승리,패배시 클라이언트 마다 보여질 위젯이 달라야 하므로
			TSubclassOf<UUserWidget> TargetClass = bWin ? Widget_WinClass : Widget_LoseClass;
			if (TargetClass)
			{
				WidgetComp->SetWidgetClass(TargetClass);
			}
		}
	}
	
	// 패배: 버튼 Visible(재도전 기능 가능) 승리: 버튼 Hidden(재도전 요쳥을 수락만 할 수 있게)
	if (Widget_ChatInst)
	{
		if (!bWin)
		{
			Widget_ChatInst->SetStartButtonEnable(true);
			Widget_ChatInst->SetStartButtonVisibility(true);
		}
		else
		{
			Widget_ChatInst->SetStartButtonVisibility(false);
		}
	}
}

// 패자 클라이언트에서 재도전 버튼을 누르고 재도전 요청 위젯을 띄우는 ClientRPC
void ABBPlayerController::ClientShowRematchRequest_Implementation()
{
	if (Widget_RematchRequestClass)
	{
		Widget_RematchRequestInst = CreateWidget<UUserWidget>(this, Widget_RematchRequestClass);
		if (Widget_RematchRequestInst)
		{
			Widget_RematchRequestInst->AddToViewport();
		}
	}
}

// 패자 클라이언트 재도전 요청 -> 승자 클라이언트에서 거절
void ABBPlayerController::ClientOnRematchDeclined_Implementation()
{
	if (Widget_ChatInst)
	{
		Widget_ChatInst->AddMessage(TEXT("[System] : 재도전 요청이 거절되었습니다."));
	}
}

// 승자 : 재도전 요청 수락 -> 다시 위젯 리셋
void ABBPlayerController::ClientOnRoundReset_Implementation()
{
	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("TVScoreBoard"),FoundActor);
	
	if (FoundActor.Num() > 0)
	{
		UWidgetComponent* WidgetComp = FoundActor[0]->FindComponentByClass<UWidgetComponent>();
		if (WidgetComp && Widget_ScoreboardClass)
		{
			// 원래의 전광판 화면으로 복귀
			WidgetComp->SetWidgetClass(Widget_ScoreboardClass);
			
		}
	}
	
	// 게임 진행중에는 -> 버튼 Hidden
	if (Widget_ChatInst)
	{
		Widget_ChatInst->SetStartButtonVisibility(false);
	}
	
	// 재도전 요청 위젯 닫기
	if (Widget_RematchRequestInst)
	{
		Widget_RematchRequestInst->RemoveFromParent();
		Widget_RematchRequestInst = nullptr;
	}
}

// 재도전 요청에 따른 서버 RPC
void ABBPlayerController::ServerRequestRematch_Implementation()
{
	ABBGameMode* GameMode = Cast<ABBGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->RequestRematch(this);
	}
}

void ABBPlayerController::ServerRespondRematch_Implementation(bool bAccepted)
{
	ABBGameMode* GameMode = Cast<ABBGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->RespondRematch(this, bAccepted);
	}
}

void ABBPlayerController::ServerSetReady_Implementation()
{
	ABBGameMode* GM = Cast<ABBGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->SetPlayerReady(this);
	}
}