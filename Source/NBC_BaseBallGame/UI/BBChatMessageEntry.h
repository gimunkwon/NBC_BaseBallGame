#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBChatMessageEntry.generated.h"

UCLASS()
class NBC_BASEBALLGAME_API UBBChatMessageEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_Message;
	
	UFUNCTION(BlueprintCallable)
	void SetMessage(const FString& InMessage);
};
