#include "WBP_GameOver2Base.h"
#include "Components/TextBlock.h"

void UWBP_GameOver2Base::SetScore(int32 Score)
{
    if (ScoreText)
    {
        ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), Score)));
    }
}
