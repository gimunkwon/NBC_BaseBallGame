#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BBGameState.generated.h"

UENUM(BlueprintType)
enum class EBBGamePhase: uint8
{
	Waiting,	// 게임 시작전
	Playing,	// 게임 플레이 중
	Win,		// 게임 승리
	Lose		// 게임 패배
};

UCLASS()
class NBC_BASEBALLGAME_API ABBGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ABBGameState();
	
	// 남은 추측 횟수
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainingAttempts;
	// 현재 게임 진행 상황
	UPROPERTY(Replicated,BlueprintReadOnly)
	EBBGamePhase GamePhase;
	
#pragma region Override
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion 
};
