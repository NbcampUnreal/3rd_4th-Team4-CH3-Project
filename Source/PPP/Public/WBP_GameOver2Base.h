#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WBP_GameOver2Base.generated.h"

class UTextBlock;

UCLASS()
class PPP_API UWBP_GameOver2Base : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetScore(int32 Score);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScoreText;
};
