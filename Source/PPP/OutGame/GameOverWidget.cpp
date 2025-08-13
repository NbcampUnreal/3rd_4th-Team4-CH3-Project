#include "GameOverWidget.h"
#include "Components/Button.h"
#include "../Characters/PppCharacter.h"
#include "Kismet/GameplayStatics.h"

void UGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Return_BTN)
    {
        Return_BTN->OnClicked.AddDynamic(this, &UGameOverWidget::OnReturnToMainMenuClicked);
    }
}

void UGameOverWidget::NativeDestruct
(
)
{
    if (Return_BTN)
    {
        Return_BTN->OnClicked.RemoveDynamic(this, &UGameOverWidget::OnReturnToMainMenuClicked);
    }
    Super::NativeDestruct();
}

void UGameOverWidget::OnReturnToMainMenuClicked()
{
    // 레벨 이동 전 일시정지 해제
    if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
    {
        PC->SetPause(false);
    }

    UGameplayStatics::OpenLevel(this, FName("MainMenuLevel"));
}

void UGameOverWidget::HandlePlayerDeath()
{}
