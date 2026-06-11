#include "BBChatWidget.h"

#include "BBChatMessageEntry.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"

void UBBChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_SendChat->OnClicked.AddDynamic(this, &UBBChatWidget::OnSendButtonClicked);
	ET_InputChat->OnTextCommitted.AddDynamic(this, &UBBChatWidget::OnInputTextCommitted);
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

// 버튼 입력 발생시 AddMessage 메서드호출
// EditableTextBox를 비워줌
void UBBChatWidget::OnSendButtonClicked()
{
	const FString Message = ET_InputChat->GetText().ToString();
	
	if (!Message.IsEmpty())
	{
		AddMessage(Message);
		ET_InputChat->SetText(FText::GetEmpty());
	}
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
