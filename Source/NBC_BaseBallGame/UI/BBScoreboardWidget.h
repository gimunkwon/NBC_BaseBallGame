#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBScoreboardWidget.generated.h"

class UImage;

UCLASS()
class NBC_BASEBALLGAME_API UBBScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:	
#pragma region Setter
	UFUNCTION()
	void SetStrikeCount(int32 count);
	UFUNCTION()
	void SetBallCount(int32 count);
	UFUNCTION()
	void SetOutCount(bool bIsOut);
#pragma endregion 
	
protected:
#pragma region BindWidget
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Strike_0;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Strike_1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Strike_2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Ball_0;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Ball_1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Ball_2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Out_0;
#pragma endregion 
	
private:
	// LitCount 만큼 켜고 나머지는 끄는 공통 헬퍼 메서드
	void UpdateRow(const TArray<UImage*>& Indicators, int32 LitCount, FLinearColor ActivateColor);
	
};
