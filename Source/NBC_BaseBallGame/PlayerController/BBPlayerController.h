#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBPlayerController.generated.h"


UCLASS()
class NBC_BASEBALLGAME_API ABBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//TODO::ClientRPC 함수
	// 즉 대화를 출력할 메서드
	UFUNCTION(Client,Reliable)
	void ClientReceiveChat(const FString& SenderName, const FString& Message);
};
