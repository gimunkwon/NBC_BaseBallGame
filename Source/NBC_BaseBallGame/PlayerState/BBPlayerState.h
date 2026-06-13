#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BBPlayerState.generated.h"

UCLASS()
class NBC_BASEBALLGAME_API ABBPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsReady = false;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
