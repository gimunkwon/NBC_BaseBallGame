#include "BBChatMessageEntry.h"

#include "Components/TextBlock.h"

void UBBChatMessageEntry::SetMessage(const FString& InMessage)
{
	if (Text_Message)
	{
		Text_Message->SetText(FText::FromString(InMessage));
	}
}
