#include "BBPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NBC_BaseBallGame/UI/BBChatWidget.h"

void ABBPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (Widget_ChatClass)
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
	UKismetSystemLibrary::PrintString(this, LastString, true, true, FLinearColor::Green, 5.f);
}