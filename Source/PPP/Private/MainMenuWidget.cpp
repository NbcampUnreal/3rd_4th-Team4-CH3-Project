#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OutGamePlayerController.h"

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
    UE_LOG(LogTemp, Log, TEXT("Start Clicked"));

    if (AOutGamePlayerController* PC = Cast<AOutGamePlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->StartGame();
    }
}

void UMainMenuWidget::OnQuitClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Quit Clicked"));
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}
