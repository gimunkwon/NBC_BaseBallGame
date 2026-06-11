#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBPlayerController.generated.h"


class UBBChatWidget;

UCLASS()
class NBC_BASEBALLGAME_API ABBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
#pragma region ChatFunc
	UFUNCTION(Client,Reliable)
	void ClientReceiveChat(const FString& SenderName, const FString& Message);
#pragma endregion 
	
protected:
#pragma region Override
	virtual void BeginPlay() override;
#pragma endregion 
	
#pragma region Widget
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI")
	TSubclassOf<UUserWidget> Widget_ChatClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI|Instance")
	TObjectPtr<UBBChatWidget> Widget_ChatInst;
#pragma endregion 
};
