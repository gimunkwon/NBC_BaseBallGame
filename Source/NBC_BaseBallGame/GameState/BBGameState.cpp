#include "BBGameState.h"

#include "Net/UnrealNetwork.h"

ABBGameState::ABBGameState()
	: RemainingAttempts(10)
	, GamePhase(EBBGamePhase::Waiting)
	, LastGuessStrikes(0)
	, LastGuessBalls(0)
{
	
}

void ABBGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, RemainingAttempts);
	DOREPLIFETIME(ThisClass, GamePhase);
	DOREPLIFETIME(ThisClass, LastGuessBalls);
	DOREPLIFETIME(ThisClass, LastGuessStrikes);
}

void ABBGameState::SetLastGuessResult(int32 Strikes, int32 Balls)
{
	LastGuessStrikes = Strikes;
	LastGuessBalls  = Balls;
	
	const bool bIsOut = (Strikes == 0 && Balls == 0);
	OnGuessResultUpdated.Broadcast(LastGuessStrikes,LastGuessBalls,bIsOut);
}

void ABBGameState::OnRep_GuessResult()
{
	const bool bIsOut = (LastGuessStrikes == 0 && LastGuessBalls == 0);
	OnGuessResultUpdated.Broadcast(LastGuessStrikes,LastGuessBalls,bIsOut);
}
