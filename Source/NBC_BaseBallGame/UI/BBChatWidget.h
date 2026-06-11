#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBChatWidget.generated.h"

class UBBChatMessageEntry;
class UButton;
class UEditableTextBox;
class UScrollBox;

UCLASS()
class NBC_BASEBALLGAME_API UBBChatWidget : public UUserWidget
{
	GENERATED_BODY()
public:
#pragma region BindWidget
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_ChatLog;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> ET_InputChat;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_SendChat;
#pragma endregion 
	
#pragma region WidgetClass
	// 동적스폰할 채팅로그위젯
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBBChatMessageEntry> ChatMessageEntryClass;
#pragma endregion
	
	UFUNCTION(BlueprintCallable)
	void AddMessage(const FString& InMessage);
	
private:
	UFUNCTION()
	void OnSendButtonClicked();
	
	UFUNCTION()
	void OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
protected:
#pragma region Override
	virtual void NativeConstruct() override;
#pragma endregion 
};
