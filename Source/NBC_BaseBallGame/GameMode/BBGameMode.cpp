#include "BBGameMode.h"

#include "GameFramework/PlayerState.h"
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
}

void ABBGameMode::BroadCastChat(const FString& SenderName, const FString& Message)
{
	//TODO::모든 플레이어 컨트롤러 이터레이터로 찾아서 컨트롤러의 PrintString을 할수있는 메서드 호출
	// ClientRPC를 쏘는거지
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABBPlayerController* PC = Cast<ABBPlayerController>(*It);
		if (PC)
		{
			PC->ClientReceiveChat(SenderName,Message);
		}
	}
}
