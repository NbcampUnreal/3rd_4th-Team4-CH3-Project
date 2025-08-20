#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameOverWidget.generated.h"

/**
 *
 */

class APppCharacter;
class UTextBlock;


UCLASS()
class PPP_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnReturnToMainMenuClicked();

public:
    UPROPERTY(meta = (BindWidget))
    UButton* Return_BTN;

    // 점수 표시할 TextBlock 변수 추가
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ScoreText;

    // 게임 오버 위젯에 점수를 설정하는 함수 추가
    void SetFinalScore(int32 FinalScore);

    UFUNCTION()
    void HandlePlayerDeath();

private:
    APppCharacter* CachedCharacter;

#pragma region ScoreText탁
public:




#pragma endregion

};
