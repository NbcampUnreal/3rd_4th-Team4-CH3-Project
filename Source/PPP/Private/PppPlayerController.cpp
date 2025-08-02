#include "PppPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PPPGameMode.h"

APppPlayerController::APppPlayerController()
    : InputMappingContext(nullptr)
    , MoveAction(nullptr)
    , JumpAction(nullptr)
    , LookAction(nullptr)
    , SprintAction(nullptr)
    , CrouchAction(nullptr)
    , PovChangeAction(nullptr)
    , PickUpAction(nullptr)
    , FireAction(nullptr)
    , MainMenuWidgetClass(nullptr)
    , MainMenuWidgetInstance(nullptr)
    , PauseMenuWidgetClass(nullptr)
    , PauseMenuWidgetInstance(nullptr)
    , PauseMenuAction(nullptr)
    , PauseMenuIMC(nullptr)
    , QuitSound(nullptr)
    , GameOverWidgetClass(nullptr)
    , GameOverWidgetInstance(nullptr)
{
}

void APppPlayerController::BeginPlay()
{
    Super::BeginPlay();

    EnableInput(this);  // ESC나 메뉴 입력용

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                SubSystem->AddMappingContext(InputMappingContext, 0);
                UE_LOG(LogTemp, Warning, TEXT("InputMappingContext 등록 완료"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("InputMappingContext가 설정되지 않았습니다."));
            }

            if (PauseMenuIMC)
            {
                SubSystem->AddMappingContext(PauseMenuIMC, 0);
                UE_LOG(LogTemp, Warning, TEXT("PauseMenuIMC 등록 완료"));
            }
        }
    }

    FString CurrentMapName = GetWorld()->GetMapName();

    if (CurrentMapName.Contains("MainMenuLevel"))
    {
        ShowMainMenu(false);
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
    //인풋 모드를 GameOnly로 바꿔서 키보드 입력 (WASD 등) 가능하게
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;

    //맵 로딩
    UGameplayStatics::OpenLevel(GetWorld(), FName("BasicMap"), true);

    UE_LOG(LogTemp, Warning, TEXT("StartGame() called - BasicMap으로 이동 중"));
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
