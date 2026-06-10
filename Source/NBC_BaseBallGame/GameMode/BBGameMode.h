#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BBGameMode.generated.h"

UCLASS()
class NBC_BASEBALLGAME_API ABBGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABBGameMode();
#pragma region Override
	virtual void PostLogin(APlayerController* NewPlayer) override;
#pragma endregion
	
#pragma region ChatFunc
	void BroadCastChat(const FString& SenderName, const FString& Message);
#pragma endregion 
	
private:
	int32 PlayerIndex;
};
