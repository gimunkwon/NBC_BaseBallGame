#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BBGameMode.generated.h"

class ABBPlayerController;

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

#pragma region GameLogicFunc
	// 게임시작 초기화 로직
	void Startgame();
	void SetPlayerReady(ABBPlayerController* PC);
	void CheckAllReady();

	// 정답 맞추기,게임 상태 관리
	void ProcessGuess(const FString& GuessString, ABBPlayerController* SenderPC);
	void NotifyGameResult(ABBPlayerController* WinnerPC, ABBPlayerController* LoserPC);
	void RequestRematch(ABBPlayerController* RequesterPC);
	void RespondRematch(ABBPlayerController* ResponderPC, bool bAccepted);
	void ResetRound();

	// 턴 관리
	void StartTurn();
	void OnTurnTimeout();
	void AdvanceTurn();
#pragma endregion

private:
	int32 PlayerIndex;
	// 비밀번호 배열
	TArray<int32> SecretNumber;

	// 비밀번호 생성 메서드
	void GenerateSeceretNumber();

	// 승자/패자 클라이언트 인스턴스
	TWeakObjectPtr<ABBPlayerController> CachedWinnerPC;
	TWeakObjectPtr<ABBPlayerController> CachedLoserPC;

	// 재도전 요청 중복 방지 플래그
	bool bRematchPending = false;

	// 턴 관리
	TArray<TWeakObjectPtr<ABBPlayerController>> TurnOrder;
	int32 CurrentTurnIndex = 0;
	FTimerHandle TurnTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category="GameLogic")
	float TurnDuration = 30.f;
};
