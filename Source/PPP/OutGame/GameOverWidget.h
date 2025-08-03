#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameOverWidget.generated.h"

/**
 *
 */
UCLASS()
class PPP_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnReturnToMainMenuClicked();

    UFUNCTION()
    void OnPlayerDead();

public:
    UPROPERTY(meta = (BindWidget))
    UButton* Return_BTN;
};
