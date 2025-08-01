#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

/**
 * @brief 메인 메뉴 UI 위젯을 구현하는 클래스입니다.
 * 게임 시작 버튼과 종료 버튼을 포함하고 있으며, 버튼 클릭 이벤트를 처리합니다.
 * by Team4 (yeoul)
 */
UCLASS()
class PPP_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* Start_BTN;

    UPROPERTY(meta = (BindWidget))
    UButton* Quit_BTN;

    UFUNCTION()
    void OnStartClicked();

    UFUNCTION()
    void OnQuitClicked();
};

