#include "BBGameState.h"

#include "Net/UnrealNetwork.h"

ABBGameState::ABBGameState()
	: RemainingAttempts(10)
	, GamePhase(EBBGamePhase::Waiting)
{
	
}

void ABBGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, RemainingAttempts);
	DOREPLIFETIME(ThisClass, GamePhase);
}
