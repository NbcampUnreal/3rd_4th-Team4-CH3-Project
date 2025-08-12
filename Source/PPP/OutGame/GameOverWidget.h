#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameOverWidget.generated.h"

/**
 *
 */

class APppCharacter;

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

    UFUNCTION()
    void HandlePlayerDeath();

private:
    APppCharacter* CachedCharacter;

};
