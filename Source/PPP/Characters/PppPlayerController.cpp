#include "PppPlayerController.h"
#include "GameFramework/HUD.h"
#include "PppCharacter.h"
#include "../GameMode/PGameInstance.h"
#include "GameOverWidget.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "../GameMode/PPPGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PPP/GameMode/PPPGameMode.h" // 정현성 추가
#include "TestQuestActorComponent.h" // 정현성 추가

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

// 김여울
static void AddOnceToViewport
(
    UUserWidget*& OutWidget,
    TSubclassOf<UUserWidget> ClassToCreate,
    APlayerController* PC,
    int32 ZOrder = 0
);

void APppPlayerController::BeginPlay()
{
    Super::BeginPlay();

    const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

    // === GameOver 레벨 처리 ===
    if (CurrentLevelName.Equals(TEXT("LV_GameOver")))
    {
        // 이전 레벨의 AHUD 클래스를 제거 (필요한 경우)
        if (AHUD* Hud = GetHUD())
        {
            Hud->Destroy();
        }

        // 마우스 커서 활성화 및 UI 모드로 변경
        bShowMouseCursor = true;

        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);

        // 기존 위젯 제거 (중복 방지)
        if (GameOverWidgetInstance)
        {
            GameOverWidgetInstance->RemoveFromParent();
            GameOverWidgetInstance = nullptr;
        }

        // 새 위젯 생성
        if (GameOverWidgetClass)
        {
            UGameOverWidget* Widget = CreateWidget<UGameOverWidget>(this, GameOverWidgetClass);
            GameOverWidgetInstance = Widget;

            if (Widget)
            {
                Widget->AddToViewport(9999);          // 맨 위에 표시
                Widget->SetIsFocusable(true);

                //  점수 전달
                if (UPPPGameInstance* GI = Cast<UPPPGameInstance>(GetGameInstance()))
                {
                    Widget->SetFinalScore(GI->FinalScore);
                }
            }
        }

        return; // HUD 생성 스킵
    }

    // === 메인메뉴일 때 ===
    if (CurrentLevelName.Equals(TEXT("MainMenuLevel")))
    {
        // 이전에 Pause 되어 있었으면 해제
        if (IsPaused())
        {
            SetPause(false);
        }
        bPauseOpen = false;

        // 남아 있을 수 있는 UI 위젯 정리
        if (PauseMenuWidgetInstance)
        {
            PauseMenuWidgetInstance->RemoveFromParent();
            PauseMenuWidgetInstance = nullptr;
        }
        if (GameOverWidgetInstance)
        {
            GameOverWidgetInstance->RemoveFromParent();
            GameOverWidgetInstance = nullptr;
        }

        if (AHUD* Hud = GetHUD())
        {
            Hud->Destroy();
        }

        bShowMouseCursor = true;

        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);

        ShowMainMenu(true);
        return; // 여기서 끝
    }

    // === 게임 레벨일 때 HUD 위젯들 생성 ===
    AddOnceToViewport(HealthWidget,    HealthClass,    this, 10);
    AddOnceToViewport(CrosshairWidget, CrosshairClass, this, 20);
    AddOnceToViewport(WeaponTrayWidget,WeaponTrayClass,this, 30);
    AddOnceToViewport(HitmarkerWidget, HitmarkerClass, this, 40);
    AddOnceToViewport(QuestWidget,     QuestClass,     this, 50);
    AddOnceToViewport(TimeWidget,      TimeClass,      this, 60);

    SetHudWidgetsVisible(true);

    // === 퀘스트 UI 초기화 ===
    if (QuestUIWidgetClass)
    {
        QuestUIWidgetInstance = CreateWidget<UUserWidget>(this, QuestUIWidgetClass);
        if (QuestUIWidgetInstance)
        {
            QuestUIWidgetInstance->AddToViewport();
            QuestProgressTextBlock = Cast<UTextBlock>(QuestUIWidgetInstance->GetWidgetFromName(TEXT("KillCountText")));

            if (APPPGameMode* GameMode = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this)))
            {
                if (UTestQuestActorComponent* QuestComponent = GameMode->GetQuestComponent())
                {
                    QuestComponent->OnQuestProgressUpdated.AddDynamic(this, &APppPlayerController::OnQuestProgressUpdated);
                }
            }
        }
    }

    // === 인풋 매핑 추가 ===
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                SubSystem->AddMappingContext(InputMappingContext, 0);
            }

            if (PauseMenuIMC)
            {
                SubSystem->AddMappingContext(PauseMenuIMC, 1);
            }
        }
    }
    // 이미 소유 중인 Pawn에도 안전 바인딩
    BindDeathDelegateToPawn(GetPawn());
}


void APppPlayerController::OnQuestProgressUpdated(int32 CurrentKills, int32 TargetKills)
{
    if (QuestProgressTextBlock)
    {
        FText NewText = FText::Format(FText::FromString(TEXT("{0}/{1}")), FText::AsNumber(CurrentKills), FText::AsNumber(TargetKills));
        QuestProgressTextBlock->SetText(NewText);
    }
}


void APppPlayerController::ShowMainMenu
(
    bool bIsRestart
)
{
    // Unpause + 잔여 UI 정리
    if (IsPaused())
    {
        SetPause(false);
    }
    bPauseOpen = false;

    SetHudWidgetsVisible(false); // 혹시 남아있을 HUD 접기

    if (PauseMenuWidgetInstance)
    {
        PauseMenuWidgetInstance->RemoveFromParent();
        PauseMenuWidgetInstance = nullptr;
    }
    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->RemoveFromParent();
        GameOverWidgetInstance = nullptr;
    }

    // 기존 로직 유지
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
            MainMenuWidgetInstance->AddToViewport(500); // 여유 ZOrder
            bShowMouseCursor = true;

            FInputModeUIOnly Mode;
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            SetInputMode(Mode);

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
    EnsureHudWidgets();

    // 클래스 누락 체크 (크래시 대신 경고 + 계속 진행)
    if (!ensureMsgf(PauseMenuWidgetClass != nullptr, TEXT("PauseMenuWidgetClass is null.")))
    {
        return;
    }

    // 위젯이 아직 없으면 1회 생성 (매번 만들고 지우지 말자)
    if (PauseMenuWidgetInstance == nullptr)
    {
        PauseMenuWidgetInstance = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
        ensureMsgf(PauseMenuWidgetInstance != nullptr, TEXT("Failed to create PauseMenuWidgetInstance."));

        if (PauseMenuWidgetInstance)
        {
            PauseMenuWidgetInstance->AddToViewport(1000);
            PauseMenuWidgetInstance->SetIsFocusable(true);
        }
    }

    // 보이기
    if (PauseMenuWidgetInstance)
    {
        PauseMenuWidgetInstance->SetVisibility(ESlateVisibility::Visible);
    }

    // HUD 숨기고 게임 일시정지
    SetHudWidgetsVisible(false);
    SetPause(true);

    // 마우스/포커스 UI 전환
    bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(PauseMenuWidgetInstance ? PauseMenuWidgetInstance->TakeWidget() : TSharedPtr<SWidget>());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    bPauseOpen = true;
}

void APppPlayerController::ShowGameOver()
{
    UE_LOG(LogTemp, Warning, TEXT("GameOverWidgetClass: %s"), *GetNameSafe(GameOverWidgetClass));

    // EnsureHudWidgets();

    if (!ensureMsgf(GameOverWidgetClass != nullptr, TEXT("GameOverWidgetClass is null.")))
    {
        return;
    }

    if (GameOverWidgetInstance == nullptr)
    {
        UGameOverWidget* TypedWidget = CreateWidget<UGameOverWidget>(this, GameOverWidgetClass);
        GameOverWidgetInstance = TypedWidget;
        ensureMsgf(GameOverWidgetInstance != nullptr, TEXT("Failed to create GameOverWidgetInstance."));

        if (GameOverWidgetInstance)
        {
            GameOverWidgetInstance->AddToViewport(9999);
            GameOverWidgetInstance->SetIsFocusable(true);

            // Return 버튼 바인딩
            if (UButton* ReturnButton = Cast<UButton>(GameOverWidgetInstance->GetWidgetFromName(TEXT("Return_BTN"))))
            {
                ReturnButton->OnClicked.AddDynamic(this, &APppPlayerController::StartGame);
            }
        }
    }

    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->SetVisibility(ESlateVisibility::Visible);
    }

    // 이제 이 시점에 HUD 제거
    SetHudWidgetsVisible(false);
    SetPause(true);
    bShowMouseCursor = true;

    bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(GameOverWidgetInstance ? GameOverWidgetInstance->TakeWidget() : TSharedPtr<SWidget>());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
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
    // 열려 있으면 닫기, 아니면 열기
    if (bPauseOpen)
    {
        // 닫기
        if (PauseMenuWidgetInstance)
        {
            PauseMenuWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
        }

        SetPause(false);
        SetHudWidgetsVisible(true);     // HUD 복구
        bShowMouseCursor = false;

        FInputModeGameOnly GameOnly;
        SetInputMode(GameOnly);

        bPauseOpen = false;
        return;
    }

    // 열기
    ShowPauseMenu();
}

// 캐릭터 사망 후 GameOverWidget 띄우기
// by Team4 (yeoul)
void APppPlayerController::OnCharacterDead()
{
    ShowGameOver();
}

// 김여울
static void AddOnceToViewport
(
    UUserWidget*& OutWidget,
    TSubclassOf<UUserWidget> ClassToCreate,
    APlayerController* PC,
    int32 ZOrder
)
{
    if (!IsValid(OutWidget) && *ClassToCreate)
    {
        OutWidget = CreateWidget<UUserWidget>(PC, ClassToCreate);
        if (IsValid(OutWidget))
        {
            OutWidget->AddToViewport(ZOrder);
            OutWidget->SetIsEnabled(true);
            OutWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
    }
}

// 김여울
void APppPlayerController::SetHudWidgetsVisible(bool bVisible)
{
    // HUD 위젯 제거 방식 전환: 감추는 게 아니라 완전 제거
    auto RemoveOrAdd = [this, bVisible](UUserWidget*& Widget, TSubclassOf<UUserWidget> ClassToCreate, int32 ZOrder)
    {
        if (bVisible)
        {
            if (!IsValid(Widget) && *ClassToCreate)
            {
                Widget = CreateWidget<UUserWidget>(this, ClassToCreate);
                if (IsValid(Widget))
                {
                    Widget->AddToViewport(ZOrder);
                    Widget->SetIsEnabled(true);
                    Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
                }
            }
            else if (IsValid(Widget))
            {
                Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
                Widget->SetIsEnabled(true);
            }
        }
        else
        {
            if (IsValid(Widget))
            {
                Widget->RemoveFromParent();
                Widget = nullptr;
            }
        }
    };

    RemoveOrAdd(HealthWidget,    HealthClass,    10);
    RemoveOrAdd(CrosshairWidget, CrosshairClass, 20);
    RemoveOrAdd(WeaponTrayWidget,WeaponTrayClass,30);
    RemoveOrAdd(HitmarkerWidget, HitmarkerClass, 40);
    RemoveOrAdd(QuestWidget,     QuestClass,     50);
    RemoveOrAdd(TimeWidget,      TimeClass,      60);

    if (bVisible)
    {
        bShowMouseCursor = false;
        FInputModeGameOnly GameOnly;
        SetInputMode(GameOnly);
    }
    else
    {
        bShowMouseCursor = true;
        FInputModeUIOnly UIOnly;
        UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(UIOnly);
    }
}

// 김여울
// 캐릭터가 소유될 때마다 사망 델리게이트 바인딩
void APppPlayerController::BindDeathDelegateToPawn
(
    APawn* InPawn
)
{
    if (APppCharacter* P = Cast<APppCharacter>(InPawn))
    {
        // 중복 방지 후 바인딩
        P->OnCharacterDead.RemoveDynamic(this, &APppPlayerController::OnCharacterDead);
        P->OnCharacterDead.AddDynamic(this, &APppPlayerController::OnCharacterDead);
    }
}

void APppPlayerController::OnPossess
(
    APawn* InPawn
)
{
    Super::OnPossess(InPawn);
    BindDeathDelegateToPawn(InPawn);
}

// 김여울
void APppPlayerController::EnsureHudWidgets()
{
    AddOnceToViewport(HealthWidget,    HealthClass,    this, 10);
    AddOnceToViewport(CrosshairWidget, CrosshairClass, this, 20);
    AddOnceToViewport(WeaponTrayWidget,WeaponTrayClass,this, 30);
    AddOnceToViewport(HitmarkerWidget, HitmarkerClass, this, 40);
    AddOnceToViewport(QuestWidget,     QuestClass,     this, 50);
    AddOnceToViewport(TimeWidget,      TimeClass,      this, 60);
}
