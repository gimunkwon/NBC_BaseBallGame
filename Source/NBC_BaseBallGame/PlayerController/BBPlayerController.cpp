#include "BBPlayerController.h"

#include "Kismet/KismetSystemLibrary.h"

void ABBPlayerController::ClientReceiveChat_Implementation(const FString& SenderName, const FString& Message)
{
	FString LastString = FString::Printf(TEXT("[%s] : %s"),*SenderName, *Message);
	UKismetSystemLibrary::PrintString(this, LastString, true, true, FLinearColor::Green, 5.f);
}
