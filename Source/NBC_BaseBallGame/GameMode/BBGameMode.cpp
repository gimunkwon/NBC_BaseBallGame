#include "BBGameMode.h"


#include "GameFramework/PlayerState.h"
#include "NBC_BaseBallGame/GameState/BBGameState.h"
#include "NBC_BaseBallGame/PlayerController/BBPlayerController.h"

ABBGameMode::ABBGameMode()
	: PlayerIndex(0)
{
}

void ABBGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	FString PostLogin_PlayerName = FString::Printf(TEXT("Player%d"),PlayerIndex);
	BroadCastChat(TEXT("System"), FString::Printf(TEXT("%s 님이 입장했습니다."),*PostLogin_PlayerName));
	PlayerIndex++;
	
	// 최소 2명이상의 플레이어가 접속했을때만 게임 시작
	ABBGameState* GS = GetGameState<ABBGameState>();
	if (GS && GetNumPlayers() >= 2 && GS->GamePhase == EBBGamePhase::Waiting)
	{
		Startgame();
		UE_LOG(LogTemp,Warning,TEXT("BaseBallGameStart!!"));
	}
	
	/*// 테스트용
	Startgame();*/
}

void ABBGameMode::BroadCastChat(const FString& SenderName, const FString& Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
		if (PC)
		{
			PC->ClientReceiveChat(SenderName,Message);
		}
	}
}

void ABBGameMode::Startgame()
{
	GenerateSeceretNumber();
	
	ABBGameState* GS = GetWorld()->GetGameState<ABBGameState>();
	if (GS)
	{
		GS->GamePhase = EBBGamePhase::Playing;
		GS->RemainingAttempts = 10;
	}
	
	BroadCastChat(TEXT("System"), TEXT("게임이 시작 되었습니다."));
}
// 정답유무/시도횟수 관리 메서드
void ABBGameMode::ProcessGuess(const FString& GuessString, const FString& SenderName)
{
#pragma region EarlyExit
	ABBGameState* GS = GetGameState<ABBGameState>();
	if (!GS || GS->GamePhase != EBBGamePhase::Playing)
	{
		return;
	}
	
	// 입력문자열중에 순수하게 세자리 숫자만 입력하지 않았다면
	for (const auto& GuessChar : GuessString)
	{
		if (!FChar::IsDigit(GuessChar))
		{
			BroadCastChat(TEXT("System"), TEXT("숫자만 입력해 주세요"));
			return;
		}
	}
	
	// 입력한 숫자가 세자리가 아닌 경우
	if (GuessString.Len() != 3)
	{
		FString Answer = FString::Printf(TEXT("3자리 유효한 숫자만 입력해주세요 : 현재 입력한 수의 자릿수 : %d"),GuessString.Len());
		BroadCastChat(TEXT("System"),Answer);
		return;
	}
#pragma endregion 
	
	int32 Strike = 0, Ball = 0;
	for (int32 i = 0; i < GuessString.Len(); i++)
	{
		if (GuessString[i] - '0' == SecretNumber[i])
		{
			Strike++;
		}
		else if (SecretNumber.Contains(GuessString[i] - '0'))
		{
			Ball++;
		}
	}
	GS->SetLastGuessResult(Strike,Ball);
	
	GS->RemainingAttempts--;
	
#pragma region BroadCastChat
	// 추측에 대한 결과값
	if (Strike + Ball == 0)
	{
		BroadCastChat(TEXT("System"), FString::Printf(TEXT("%s의 추측: %s -> Out | 남은 횟수: %d"),*SenderName, *GuessString,GS->RemainingAttempts));
	}
	else
	{
		FString Result = FString::Printf(TEXT("%s의 추측 : %s -> %dS %dB | 남은 횟수: %d"),*SenderName, *GuessString, Strike, Ball,GS->RemainingAttempts);
		BroadCastChat(TEXT("System"), Result);
	}
	// 승리
	if (Strike == 3)
	{
		GS->GamePhase = EBBGamePhase::Win;
		BroadCastChat(TEXT("System"), TEXT("정답입니다. 게임 클리어"));
		return;
	}
	// 패배
	if (GS->RemainingAttempts <= 0)
	{
		GS->GamePhase = EBBGamePhase::Lose;
		FString Answer = FString::Printf(TEXT("게임 오버 정답은 %d%d%d 였습니다."),SecretNumber[0],SecretNumber[1],SecretNumber[2]);
		BroadCastChat(TEXT("System"), Answer);
		return;
	}
#pragma endregion 
	
	
}
// 비밀번호 생성 메서드
void ABBGameMode::GenerateSeceretNumber()
{
	SecretNumber.Empty();
	TArray<int32> Pool = {1,2,3,4,5,6,7,8,9};
	
	for (int32 i = 0; i < 3; i++)
	{
		int32 RandomNumber = Pool[FMath::RandRange(0, Pool.Num() - 1)];
		SecretNumber.Add(RandomNumber);
		Pool.Remove(RandomNumber);
	}
	
	/*FString ChooseNumber = "";
	
	for (int32 i = 0; i < SecretNumber.Num(); i++)
	{
		ChooseNumber += FString::Printf(TEXT("%d"),SecretNumber[i]);
	}
	
	UE_LOG(LogTemp,Warning,TEXT("Current Answer : %s"),*ChooseNumber);*/
	
}
