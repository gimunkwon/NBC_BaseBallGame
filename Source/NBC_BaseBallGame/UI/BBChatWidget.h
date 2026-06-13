#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBChatWidget.generated.h"

class UTextBlock;
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
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ModeDisplay;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_ReadyORReset;
#pragma endregion 
	
#pragma region WidgetClass
	// 동적스폰할 채팅로그위젯
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBBChatMessageEntry> ChatMessageEntryClass;
#pragma endregion
	
	UFUNCTION(BlueprintCallable)
	void AddMessage(const FString& InMessage);
	UFUNCTION(BlueprintCallable)
	void SetStartButtonVisibility(bool bVisible);
	UFUNCTION(BlueprintCallable)
	void SetStartButtonEnable(bool bCanEnable);
	
private:
	UFUNCTION()
	void OnSendButtonClicked();
	UFUNCTION()
	void OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void OnStartGameButtonClicked();
	
	bool bIsGuessMode = false;
	
protected:
#pragma region Override
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
#pragma endregion 
};
