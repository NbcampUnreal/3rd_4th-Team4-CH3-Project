#include "PppPlayerController.h"
#include "GameFramework/HUD.h"
#include "PppCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

APppPlayerController::APppPlayerController()
    : InputMappingContext(nullptr)
    , MoveAction(nullptr)
    , JumpAction(nullptr)
    , LookAction(nullptr)
    , ZoomAction(nullptr)
    , SprintAction(nullptr)
    , CrouchAction(nullptr)
    , PovChangeAction(nullptr)
    , PickUpAction(nullptr)
    , FireAction(nullptr)
    , ReloadAction(nullptr)
    , EquipRifleAction(nullptr)
    , PauseMenuIMC(nullptr)
    , PauseMenuAction(nullptr)
    , MainMenuWidgetClass(nullptr)
    , PauseMenuWidgetClass(nullptr)
    , GameOverWidgetClass(nullptr)
    , MainMenuWidgetInstance(nullptr)
    , PauseMenuWidgetInstance(nullptr)
    , GameOverWidgetInstance(nullptr)
    , QuitSound(nullptr)
{
}


void APppPlayerController::BeginPlay()
{
    Super::BeginPlay();

    const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true); // PIE 접두사 제거

    // MainMenuLevel에선 HUD 숨기기
    if (CurrentLevelName.Equals(TEXT("MainMenuLevel")))
    {
        if (AHUD* Hud = GetHUD())
        {
            Hud->Destroy();
        }

        bShowMouseCursor = true;

        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);

        ShowMainMenu(true);
    }

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                SubSystem->AddMappingContext(InputMappingContext, 0);
                UE_LOG(LogTemp, Warning, TEXT("InputMappingContext 등록 완료: %s"), *InputMappingContext->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("InputMappingContext가 설정되지 않았습니다."));
            }

            if (PauseMenuIMC)
            {
                SubSystem->AddMappingContext(PauseMenuIMC, 1); // 메뉴가 게임 입력보다 우선
                UE_LOG(LogTemp, Warning, TEXT("PauseMenuIMC 등록 완료: %s"), *PauseMenuIMC->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("PauseMenuIMC가 NULL입니다!"));
            }
        }
    }
}


void APppPlayerController::ShowMainMenu(bool bIsRestart)
{
    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    if (MainMenuWidgetClass)
    {
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        if (MainMenuWidgetInstance)
        {
            MainMenuWidgetInstance->AddToViewport();
            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());

            if (UButton* StartBtn = Cast<UButton>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("Start_BTN"))))
            {
                StartBtn->OnClicked.AddDynamic(this, &APppPlayerController::StartGame);
            }
            if (UButton* QuitBtn = Cast<UButton>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("Quit_BTN"))))
            {
                QuitBtn->OnClicked.AddDynamic(this, &APppPlayerController::QuitGame);
            }
        }
    }
}

void APppPlayerController::ShowPauseMenu()
{
    if (PauseMenuWidgetInstance)
    {
        PauseMenuWidgetInstance->RemoveFromParent();
        PauseMenuWidgetInstance = nullptr;
    }

    if (PauseMenuWidgetClass)
    {
        PauseMenuWidgetInstance = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
        if (PauseMenuWidgetInstance)
        {
            PauseMenuWidgetInstance->AddToViewport();
            PauseMenuWidgetInstance->SetIsFocusable(false);

            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(PauseMenuWidgetInstance->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

            SetInputMode(InputMode);
            bShowMouseCursor = true;
        }
    }

    if (!IsPaused())
    {
        SetPause(true);
    }
}

void APppPlayerController::ShowGameOver()
{
    if (!GameOverWidgetInstance && GameOverWidgetClass)
    {
        GameOverWidgetInstance = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
        if (GameOverWidgetInstance)
        {
            GameOverWidgetInstance->AddToViewport();
            SetInputMode(FInputModeUIOnly());
            bShowMouseCursor = true;
            SetPause(true);
        }
    }
}

void APppPlayerController::StartGame()
{
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;

    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    const FName TargetLevel = StageLevelName.IsNone()
        ? FName(TEXT("Stage2"))
        : StageLevelName;

    FTimerHandle StartHandle;
    GetWorld()->GetTimerManager().SetTimer
    (
        StartHandle,
        [this, TargetLevel]()
        {
            UGameplayStatics::OpenLevel
            (
                this,
                TargetLevel
            );
        },
        0.3f,
        false
    );
}



void APppPlayerController::QuitGame()
{
    UWorld* World = GetWorld();
    if (!World) return;

    if (QuitSound)
    {
        UGameplayStatics::PlaySound2D(this, QuitSound);
    }

    FTimerHandle QuitHandle;
    World->GetTimerManager().SetTimer(
        QuitHandle,
        [this, World]() {
            APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
            UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, false);
        },
        1.0f, false);
}

void APppPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (PauseMenuAction)
        {
            EnhancedInput->BindAction(PauseMenuAction, ETriggerEvent::Started, this, &APppPlayerController::HandlePauseKey);
        }
    }
}

void APppPlayerController::HandlePauseKey()
{
    ShowPauseMenu();
}

// 캐릭터 사망 후 GameOverWidget 띄우기
// by Team4 (yeoul)
void APppPlayerController::OnCharacterDead()
{
    ShowGameOver();
}
