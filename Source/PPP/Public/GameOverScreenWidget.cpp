// GameOverScreenWidget.cpp

#include "GameOverScreenWidget.h"
#include "Components/TextBlock.h"

void UGameOverScreenWidget::SetFinalScore(int32 Score)
{
    if (ScoreText)
    {
        FString ScoreString = FString::Printf(TEXT("점수: %d"), Score);
        ScoreText->SetText(FText::FromString(ScoreString));
    }
}
