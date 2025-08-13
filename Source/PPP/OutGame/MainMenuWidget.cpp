#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../characters/PppPlayerController.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Start_BTN)
    {
        Start_BTN->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartClicked);
    }

    if (Quit_BTN)
    {
        Quit_BTN->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
    }
}

void UMainMenuWidget::OnStartClicked()
{
    if (APppPlayerController* PC = Cast<APppPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->StartGame();
    }
}

void UMainMenuWidget::OnQuitClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}
