#include "BBGameMode.h"

#include "GameFramework/PlayerState.h"
#include "NBC_BaseBallGame/GameState/BBGameState.h"
#include "NBC_BaseBallGame/PlayerController/BBPlayerController.h"
#include "NBC_BaseBallGame/PlayerState/BBPlayerState.h"

ABBGameMode::ABBGameMode()
	: PlayerIndex(0)
{
	PlayerStateClass = ABBPlayerState::StaticClass();
}

void ABBGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	FString PostLogin_PlayerName = FString::Printf(TEXT("Player%d"), PlayerIndex);
	BroadCastChat(TEXT("System"), FString::Printf(TEXT("%s 님이 입장했습니다."), *PostLogin_PlayerName));
	PlayerIndex++;

	/*// 최소 2명이상의 플레이어가 접속했을때만 게임 시작
	ABBGameState* GS = GetGameState<ABBGameState>();
	if (GS && GetNumPlayers() >= 2 && GS->GamePhase == EBBGamePhase::Waiting)
	{
		Startgame();
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
			if (PC)
			{
				PC->ClientOnRoundReset();
			}
		}
		UE_LOG(LogTemp,Warning,TEXT("BaseBallGameStart!!"));
	}*/
}

void ABBGameMode::BroadCastChat(const FString& SenderName, const FString& Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
		if (PC)
		{
			PC->ClientReceiveChat(SenderName, Message);
		}
	}
}

void ABBGameMode::Startgame()
{
	GenerateSeceretNumber();

	// 턴 순서 구성
	TurnOrder.Empty();
	CurrentTurnIndex = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABBPlayerController* PC = Cast<ABBPlayerController>(*It))
		{
			TurnOrder.Add(PC);
		}
	}

	ABBGameState* GS = GetWorld()->GetGameState<ABBGameState>();
	if (GS)
	{
		GS->GamePhase = EBBGamePhase::Playing;
		GS->RemainingAttempts = 10;
	}

	BroadCastChat(TEXT("System"), TEXT("게임이 시작 되었습니다."));
	StartTurn();
}

// 정답유무/시도횟수 관리 메서드
void ABBGameMode::ProcessGuess(const FString& GuessString, ABBPlayerController* SenderPC)
{
#pragma region EarlyExit
	ABBGameState* GS = GetGameState<ABBGameState>();
	if (!GS || GS->GamePhase != EBBGamePhase::Playing)
	{
		return;
	}

	// 현재 턴 플레이어가 아니면 제출 불가
	if (TurnOrder.Num() > 0 && TurnOrder[CurrentTurnIndex].Get() != SenderPC)
	{
		BroadCastChat(TEXT("System"), TEXT("지금은 당신의 턴이 아닙니다."));
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
		if (GuessChar == '0')
		{
			BroadCastChat(TEXT("System"), TEXT("0은 입력할 수 없습니다."));
			return;
		}
	}

	// 입력한 숫자가 세자리가 아닌 경우
	if (GuessString.Len() != 3)
	{
		FString Answer = FString::Printf(TEXT("3자리 유효한 숫자만 입력해주세요 : 현재 입력한 수의 자릿수 : %d"), GuessString.Len());
		BroadCastChat(TEXT("System"), Answer);
		return;
	}

	// 입력한 숫자가 중복된 숫자인 경우
	TSet<TCHAR> UniqueChars;
	for (const auto& GuessChar : GuessString)
	{
		bool bAlreadyExists = false;
		UniqueChars.Add(GuessChar, &bAlreadyExists);
		if (bAlreadyExists)
		{
			BroadCastChat(TEXT("System"), TEXT("중복된 숫자는 입력할 수 없습니다."));
			return;
		}
	}
#pragma endregion

#pragma region GuessResult
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
	GS->SetLastGuessResult(Strike, Ball);

	GS->RemainingAttempts--;
#pragma endregion

#pragma region BroadCastChat
	// 추측에 대한 결과값
	if (Strike + Ball == 0)
	{
		BroadCastChat(TEXT("System"), FString::Printf(TEXT("%s의 추측: %s -> Out | 남은 횟수: %d"),
			*SenderPC->PlayerState->GetPlayerName(), *GuessString, GS->RemainingAttempts));
	}
	else
	{
		FString Result = FString::Printf(TEXT("%s의 추측 : %s -> %dS %dB | 남은 횟수: %d"),
			*SenderPC->PlayerState->GetPlayerName(), *GuessString, Strike, Ball, GS->RemainingAttempts);
		BroadCastChat(TEXT("System"), Result);
	}

	// 승리
	if (Strike == 3)
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		GS->GamePhase = EBBGamePhase::Win;
		BroadCastChat(TEXT("System"), TEXT("정답입니다. 게임 클리어"));

		ABBPlayerController* LoserPC = nullptr;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
			if (PC && PC != SenderPC)
			{
				LoserPC = PC;
				break;
			}
		}
		NotifyGameResult(SenderPC, LoserPC);
		return;
	}

	// 패배
	if (GS->RemainingAttempts <= 0)
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		GS->GamePhase = EBBGamePhase::Lose;
		FString Answer = FString::Printf(TEXT("게임 오버 정답은 %d%d%d 였습니다."),
			SecretNumber[0], SecretNumber[1], SecretNumber[2]);
		BroadCastChat(TEXT("System"), Answer);

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
			if (PC)
			{
				PC->ClientOnGameResult(false);
			}
		}
		return;
	}

	// 정상 제출 → 다음 턴으로
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	AdvanceTurn();
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
		ChooseNumber += FString::Printf(TEXT("%d"), SecretNumber[i]);
	}
	UE_LOG(LogTemp,Warning,TEXT("Current Answer : %s"), *ChooseNumber);*/
}

// 게임 결과를 Client에게 알려주는 서버 함수
void ABBGameMode::NotifyGameResult(ABBPlayerController* WinnerPC, ABBPlayerController* LoserPC)
{
	CachedWinnerPC = WinnerPC;
	CachedLoserPC = LoserPC;

	if (WinnerPC)
	{
		WinnerPC->ClientOnGameResult(true);
	}
	if (LoserPC)
	{
		LoserPC->ClientOnGameResult(false);
	}
}

// 패자 클라이언트 -> 승자 클라이언트에게 재도전 요청을 처리하는 서버 함수
void ABBGameMode::RequestRematch(ABBPlayerController* RequesterPC)
{
	if (bRematchPending)
	{
		return;
	}
	bRematchPending = true;

	if (CachedWinnerPC.IsValid())
	{
		CachedWinnerPC->ClientShowRematchRequest();
	}
	else // 전원 패배 케이스
	{
		CachedLoserPC = RequesterPC;

		ABBPlayerController* OtherPC = nullptr;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
			if (PC && PC != RequesterPC)
			{
				OtherPC = PC;
				break;
			}
		}

		if (OtherPC)
		{
			OtherPC->ClientShowRematchRequest();
		}
		else
		{
			ResetRound();
		}
	}
}

// 승자 클라이언트에서 재도전 요청 수락/거절 메서드
void ABBGameMode::RespondRematch(ABBPlayerController* ResponderPC, bool bAccepted)
{
	if (bAccepted)
	{
		ResetRound();
	}
	else
	{
		bRematchPending = false;
		if (CachedLoserPC.IsValid())
		{
			CachedLoserPC->ClientOnRematchDeclined();
		}
	}
}

void ABBGameMode::ResetRound()
{
	bRematchPending = false;
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	TurnOrder.Empty();
	CurrentTurnIndex = 0;

	Startgame();

	CachedLoserPC.Reset();
	CachedWinnerPC.Reset();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
		if (PC)
		{
			PC->ClientOnRoundReset();
		}
	}
}

// 플레이어 준비 상태 확인 메서드
void ABBGameMode::SetPlayerReady(ABBPlayerController* PC)
{
	if (!PC || !PC->PlayerState)
	{
		return;
	}

	ABBPlayerState* BBPS = Cast<ABBPlayerState>(PC->PlayerState);
	// 이미 준비 완료 됬으면 EarlyExit
	if (!BBPS || BBPS->bIsReady)
	{
		return;
	}
	BBPS->bIsReady = true;

	FString PlayerName = PC->PlayerState->GetPlayerName();
	BroadCastChat(TEXT("System"), FString::Printf(TEXT("%s 님이 준비 완료했습니다."), *PlayerName));

	CheckAllReady();
}

// 플레이어의 준비 상태에 따라 StartPlay호출 여부 메서드
void ABBGameMode::CheckAllReady()
{
	ABBGameState* GS = GetGameState<ABBGameState>();
	if (!GS || GS->GamePhase != EBBGamePhase::Waiting || GetNumPlayers() < 2)
	{
		return;
	}

	for (APlayerState* PS : GS->PlayerArray)
	{
		ABBPlayerState* BBPS = Cast<ABBPlayerState>(PS);
		if (!BBPS || !BBPS->bIsReady)
		{
			return;
		}
	}

	Startgame();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
		if (PC)
		{
			PC->ClientOnRoundReset();
		}
	}
}

// 현재 턴 플레이어에게 턴 시작 알림 및 타이머 시작
void ABBGameMode::StartTurn()
{
	if (TurnOrder.Num() == 0) return;

	ABBPlayerController* CurrentPC = TurnOrder[CurrentTurnIndex].Get();
	FString TurnPlayerName = (CurrentPC && CurrentPC->PlayerState)
		? CurrentPC->PlayerState->GetPlayerName()
		: TEXT("Unknown");
	BroadCastChat(TEXT("System"), FString::Printf(TEXT("%s의 턴입니다. (제한시간: %.0f초)"), *TurnPlayerName, TurnDuration));

	for (int32 i = 0; i < TurnOrder.Num(); i++)
	{
		ABBPlayerController* PC = TurnOrder[i].Get();
		if (PC)
		{
			PC->ClientOnTurnStarted(i == CurrentTurnIndex, TurnDuration);
		}
	}

	GetWorldTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ABBGameMode::OnTurnTimeout,
		TurnDuration,
		false
	);
}

// 턴 제한 시간 초과 시 처리
void ABBGameMode::OnTurnTimeout()
{
	ABBGameState* GS = GetGameState<ABBGameState>();
	if (!GS) return;

	GS->RemainingAttempts--;
	BroadCastChat(TEXT("System"), FString::Printf(TEXT("시간 초과! 남은 횟수: %d"), GS->RemainingAttempts));

	if (GS->RemainingAttempts <= 0)
	{
		GS->GamePhase = EBBGamePhase::Lose;
		BroadCastChat(TEXT("System"), FString::Printf(TEXT("게임 오버 정답은 %d%d%d 였습니다."),
			SecretNumber[0], SecretNumber[1], SecretNumber[2]));

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (ABBPlayerController* PC = Cast<ABBPlayerController>(*It))
			{
				PC->ClientOnGameResult(false);
			}
		}
		return;
	}

	AdvanceTurn();
}

// 다음 플레이어 턴으로 전환
void ABBGameMode::AdvanceTurn()
{
	if (TurnOrder.Num() == 0) return;
	CurrentTurnIndex = (CurrentTurnIndex + 1) % TurnOrder.Num();
	StartTurn();
}
