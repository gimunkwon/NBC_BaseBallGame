#include "BBPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NBC_BaseBallGame/GameMode/BBGameMode.h"
#include "NBC_BaseBallGame/UI/BBChatWidget.h"

// void ABBPlayerController::BeginPlay()
// {
// 	Super::BeginPlay();
// }

ABBPlayerController::ABBPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ABBPlayerController::PostNetInit()
{
	Super::PostNetInit();
	
	UE_LOG(LogTemp,Warning,TEXT("PostNetInit called | IsLocal : %d | WidgetClass: %d"),IsLocalController(), Widget_ChatClass != nullptr);
	
	if (IsLocalController() && Widget_ChatClass)
	{
		Widget_ChatInst = CreateWidget<UBBChatWidget>(this, Widget_ChatClass);
		if (Widget_ChatInst)
		{
			Widget_ChatInst->AddToViewport();
		}
	}
}

void ABBPlayerController::ClientReceiveChat_Implementation(const FString& SenderName, const FString& Message)
{
	FString LastString = FString::Printf(TEXT("[%s] : %s"),*SenderName, *Message);
	// UKismetSystemLibrary::PrintString(this, LastString, true, true, FLinearColor::Green, 5.f);
	
	if (Widget_ChatInst)
	{
		Widget_ChatInst->AddMessage(LastString);
	}
}

void ABBPlayerController::ServerSendChat_Implementation(const FString& Message)
{
	ABBGameMode* GameMode = Cast<ABBGameMode>(GetWorld()->GetAuthGameMode());
	
	if (GameMode)
	{
		FString SenderName = PlayerState ? PlayerState->GetPlayerName() : TEXT("Unknown");
		GameMode->BroadCastChat(SenderName, Message);
	}
}


