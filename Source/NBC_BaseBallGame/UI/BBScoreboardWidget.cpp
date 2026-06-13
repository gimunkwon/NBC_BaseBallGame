#include "BBScoreboardWidget.h"

#include "Components/Image.h"


static const FLinearColor DimColor(0.05f, 0.05f, 0.05f, 1.f);

void UBBScoreboardWidget::SetStrikeCount(int32 count)
{
	UpdateRow({Strike_0,Strike_1,Strike_2}, count, FLinearColor::Yellow);
}

void UBBScoreboardWidget::SetBallCount(int32 count)
{
	UpdateRow({Ball_0,Ball_1,Ball_2}, count, FLinearColor::Green);
}

void UBBScoreboardWidget::SetOutCount(bool bIsOut)
{
	UpdateRow({Out_0}, bIsOut ? 1 : 0, FLinearColor::Red);
}

void UBBScoreboardWidget::UpdateRow(const TArray<UImage*>& Indicators, int32 LitCount, FLinearColor ActivateColor)
{
	for (int32 i = 0; i < Indicators.Num(); i++)
	{
		if (!Indicators[i]) continue;
		
		Indicators[i]->SetColorAndOpacity(i < LitCount ? ActivateColor : DimColor);
		//TODO:: 타이머로 꺼지는 기능추가?
	}
}
