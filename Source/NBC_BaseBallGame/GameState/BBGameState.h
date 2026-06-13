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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGuessResultUpdated, int32, Strikes, int32, Balls, bool, bIsOut);

UCLASS()
class NBC_BASEBALLGAME_API ABBGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ABBGameState();
#pragma region GameLogicValue
	// 남은 추측 횟수
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainingAttempts;
	// 현재 게임 진행 상황
	UPROPERTY(Replicated,BlueprintReadOnly)
	EBBGamePhase GamePhase;
#pragma endregion 
	
#pragma region Delegate
	UPROPERTY(BlueprintAssignable)
	FOnGuessResultUpdated OnGuessResultUpdated;
#pragma endregion 
	
#pragma region Override
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion
	
#pragma region GameLogicFunc
	void SetLastGuessResult(int32 Strikes, int32 Balls);
#pragma endregion 
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_GuessResult)
	int32 LastGuessStrikes;
	UPROPERTY(ReplicatedUsing=OnRep_GuessResult)
	int32 LastGuessBalls;
	
	UFUNCTION()
	void OnRep_GuessResult();
};
