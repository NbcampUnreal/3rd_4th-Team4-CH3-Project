#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PauseMenuWidget.generated.h"

/**
 * @class UPauseMenuWidget
 * @brief 게임의 일시 정지 메뉴를 관리하는 위젯 클래스입니다.
 * by Team4 (yeoul)
 */

UCLASS()
class PPP_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* Resume_BTN;

    UPROPERTY(meta = (BindWidget))
    UButton* Return_BTN;

    UFUNCTION()
    void OnResumeClicked();

    UFUNCTION()
    void OnReturnClicked();
};
