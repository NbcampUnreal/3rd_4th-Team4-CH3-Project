#include "OutGamePlayerController.h"
#include "Blueprint/UserWidget.h"   // UserWidget, CreateWidget
#include "Blueprint/WidgetBlueprintLibrary.h"   // FInputModeUIOnly, SetInputMode
#include "Kismet/GameplayStatics.h" // StartGame, GetPlayerController, PlaySound2D, OpenLevel
#include "Kismet/KismetSystemLibrary.h" // QuitGame
#include "Components/TextBlock.h"   // 위젯 버튼의 텍스트
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputTriggers.h"
#include "PPPGameMode.h"

// 생성자
// - 위젯 클래스 및 인스턴스 초기화
AOutGamePlayerController::AOutGamePlayerController()
    : MainMenuWidgetClass(nullptr),
      MainMenuWidgetInstance(nullptr),
      PauseMenuWidgetInstance(nullptr),
      PauseMenuAction(nullptr),
      PauseMenuIMC(nullptr),
      QuitSound(nullptr)
{
}

// BeginPlay Override
// - 레벨 이름 확인 후 메인 메뉴 레벨일 경우 UI 표시
// - ESC 키 입력 -> PauseMenu IMC 등록
void AOutGamePlayerController::BeginPlay()
{
    Super::BeginPlay();

    EnableInput(this);  // 비긴플레이 안에 있어야 ESC키 인식됨
    UE_LOG(LogTemp, Warning, TEXT("[BeginPlay] EnableInput 호출됨"));

    // 현재 레벨 이름 가져오기
    FString CurrentMapName = GetWorld()->GetMapName();

    // TODO: GameInstance에서 GameOver 상태 확인
    /*
    bool bShouldShowGameOver = false;
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UGameInstancePPP* MyGI = Cast<UGameInstancePPP>(GI))
        {
            if (MyGI->IsGameOver)
            {
                bShouldShowGameOver = true;
                MyGI->bIsGameOver = false;  //  한 번만 표시되게 리셋
            }
        }
    }
    */


    // 게임 실행 시 메인메뉴레벨에서 해당 UI 먼저 표시
    if (CurrentMapName.Contains("MainMenuLevel"))
    {
        ShowMainMenu(false);
    }

    // 게임 실행 중 PauseMenu의 IMC 적용
    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem && PauseMenuIMC)
    {
        Subsystem->AddMappingContext(PauseMenuIMC, 0);
        UE_LOG(LogTemp, Warning, TEXT("[BeginPlay] PauseMenuIMC 등록 완료"));
    }
}

// Main Menu UI 표시
// - 기존 위젯 제거 후 새로 생성
// - 입력 모드는 UIOnly로 설정 & 마우스 커서 표시
// - Start 버튼 텍스트는 bIsRestart 값에 따라 변경해서 Restart/Start 경우 분리
// TODO:: HUD 통합되면 HUD 제거 처리 추가 예정
// TODO: GameOver 상태에서 돌아올 때에도 여기 재활용할 것
void AOutGamePlayerController::ShowMainMenu(bool bIsRestart)
{
    // TODO: HUD 합치고 주석 해제하기
    // HUD 켜져 있으면 닫기
    /*
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HudWidegetInstance = nullptr;
    }
    */

    // 이미 메인메뉴 떠있으면 제거하기
    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    // 메뉴 UI 표시
    if (MainMenuWidgetClass)
    {
        // 위젯 생성하기
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        if (MainMenuWidgetInstance)
        {
            // 뷰포트에 표시하기
            MainMenuWidgetInstance->AddToViewport();
            // 마우스 커서 보이게 -> 버튼 클릭
            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }

        // 버튼 바인딩
        if (UButton* StartBtn = Cast<UButton>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("Start_BTN"))))
        {
            StartBtn->OnClicked.AddDynamic(this, &AOutGamePlayerController::StartGame);
        }
        if (UButton* QuitBtn = Cast<UButton>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("Quit_BTN"))))
        {
            QuitBtn->OnClicked.AddDynamic(this, &AOutGamePlayerController::QuitGame);
        }
    }
}

// TODO: 추후 HUD 위에 Overlay로 Pause 띄우게 바꾸는 것도 고려
// Pause Menu UI 표시
void AOutGamePlayerController::ShowPauseMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("[ShowPauseMenu] ESC 눌려서 호출됨"));

    // 기존 위젯 제거
    if (PauseMenuWidgetInstance)
    {
        PauseMenuWidgetInstance->RemoveFromParent();
        PauseMenuWidgetInstance = nullptr;
    }

    // 위젯 생성 및 표시
    if (PauseMenuWidgetClass)
    {
        PauseMenuWidgetInstance = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
        if (PauseMenuWidgetInstance)
        {
            PauseMenuWidgetInstance->AddToViewport();
            PauseMenuWidgetInstance->SetIsFocusable(false); // ESC 키 막히지 않게 설정

            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(PauseMenuWidgetInstance->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

            SetInputMode(InputMode);
            bShowMouseCursor = true;
        }
    }

    // 게임 일시정지 설정 (정지 상태에서 또 esc 누르기 방지)
    if (!IsPaused())
    {
        SetPause(true);
    }
}


// TODO: 게임 HUD 표시

// Start 버튼 -> 게임 시작 처리
// - TODO: 이후에 GameInstance 리셋 로직 추가 예정
// - TODO: Start 버튼 누르면 STage1 전환 예쩡
void AOutGamePlayerController::StartGame()
{
    // TODO: GameInstance 초기화 등은 나중에 추가

    // TODO: Stage1 완성되면 아래 줄로 교체
    // UGameplayStatics::OpenLevel(GetWorld(), FName("Stage1"));
    UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel")); // ← 지금은 임시로 BasicLevel 테스트

    UE_LOG(LogTemp, Warning, TEXT("StartGame() called - 임시로 BasicLevel로 이동 중 ~"));
}

// Quit 버튼 → 게임 종료
void AOutGamePlayerController::QuitGame()
{
    // 현재 게임 세상의 정보 가져옴
    UWorld* World = GetWorld();
    // World 없으면 종료 로직 실행 안하고 탈출 (안전 처리용)
    if (!World) return;

    // 사운드 안 나오고 바로 종료돼서 추가
    if (QuitSound)
    {
        UGameplayStatics::PlaySound2D(this, QuitSound);
    }

    // 약간의 딜레이 후 종료
    FTimerHandle QuitHandle;
    World->GetTimerManager().SetTimer(
        QuitHandle,
        [this, World]()
        {
            // 누가 입력을 받고 있는지?
            // 첫 번째 플레이어(인덱스 0)의 컨트롤러 가져옴
            APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
            // 게임 종료(게임 월드 정보, 종료시킬 대상 컨트롤러, 그냥 종료(강제), 팝업 안 띄우고 바로 종료)
            UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, false);
        },
        1.0f,   // 딜레이 시간 (초)
        false
        );
    }

// 게임 오버
void AOutGamePlayerController::ShowGameOver()
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

// ESC 입력 바인딩 (PauseMenu)
void AOutGamePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInput->BindAction(
            PauseMenuAction,
            ETriggerEvent::Started,
            this,
            &AOutGamePlayerController::HandlePauseKey);

        UE_LOG(LogTemp, Warning, TEXT("[SetupInputComponent] ESC 바인딩 완료"));
    }
}

// ESC 키 눌렀을 때 처리 함수
void AOutGamePlayerController::HandlePauseKey()
{
    UE_LOG(LogTemp, Warning, TEXT("[HandlePauseKey] ESC 눌림 감지했어~"));
    ShowPauseMenu();
}
