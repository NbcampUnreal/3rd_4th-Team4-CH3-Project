// GameOverScreenWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverScreenWidget.generated.h"

class UTextBlock;

UCLASS()
class PPP_API UGameOverScreenWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 최종 점수를 UI에 표시
    UFUNCTION(BlueprintCallable)
    void SetFinalScore(int32 Score);

protected:
    // 바인딩된 점수 표시용 텍스트 블록 (WBP에서 이름 정확히 맞춰야 함)
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScoreText;
};
