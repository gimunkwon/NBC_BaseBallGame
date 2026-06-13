#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBPlayerController.generated.h"


class UBBChatWidget;

UCLASS()
class NBC_BASEBALLGAME_API ABBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABBPlayerController();
	
#pragma region ChatFunc
	UFUNCTION(Client,Reliable)
	void ClientReceiveChat(const FString& SenderName, const FString& Message);
	UFUNCTION(Server,Reliable)
	void ServerSendChat(const FString& Message);
#pragma endregion 
	
#pragma region GameLogic
	// 숫자 야구 정답 입력 서버RPC
	UFUNCTION(Server,Reliable)
	void ServerSubmitGuess(const FString& GuessString);
	
	// ClientRPC
	UFUNCTION(Client,Reliable)
	void ClientOnGameResult(bool bWin);
	UFUNCTION(Client,Reliable)
	void ClientShowRematchRequest();
	UFUNCTION(Client,Reliable)
	void ClientOnRematchDeclined();
	UFUNCTION(Client,Reliable)
	void ClientOnRoundReset();
	
	//ServerRPC
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void ServerRequestRematch();
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void ServerRespondRematch(bool bAccepted);
	UFUNCTION(Server,Reliable)
	void ServerSetReady();
	
#pragma endregion 
	
protected:
#pragma region Override
	// virtual void BeginPlay() override;
	virtual void PostNetInit() override;
#pragma endregion 
	
#pragma region Widget
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI")
	TSubclassOf<UBBChatWidget> Widget_ChatClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI|Instance")
	TObjectPtr<UBBChatWidget> Widget_ChatInst;
	
	// BP_TVScoreBoard 교체용 위젯 클래스
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI")
	TSubclassOf<UUserWidget> Widget_ScoreboardClass;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI")
	TSubclassOf<UUserWidget> Widget_WinClass;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI")
	TSubclassOf<UUserWidget> Widget_LoseClass;
	
	// 재도전 요청 위젯
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI")
	TSubclassOf<UUserWidget> Widget_RematchRequestClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI|Instance")
	TObjectPtr<UUserWidget> Widget_RematchRequestInst;
	
	
#pragma endregion 
};
