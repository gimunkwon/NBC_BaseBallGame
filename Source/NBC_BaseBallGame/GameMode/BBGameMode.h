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
	
#pragma region GameLogicFunc
	// 게임시작 초기화 로직
	void Startgame();
	// 정답 맞추기,게임 상태 관리
	void ProcessGuess(const FString& GuessString, const FString& SenderName);
#pragma endregion 
	
private:
	int32 PlayerIndex;
	// 비밀번호 배열
	TArray<int32> SecretNumber;
	
	// 비밀번호 생성 메서드
	void GenerateSeceretNumber();
};
