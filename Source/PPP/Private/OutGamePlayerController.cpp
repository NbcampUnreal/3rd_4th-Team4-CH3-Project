#include "OutGamePlayerController.h"
#include "Blueprint/UserWidget.h"   // UserWidget, CreateWidget
#include "Blueprint/WidgetBlueprintLibrary.h"   // FInputModeUIOnly, SetInputMode
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"   // 위젯 버튼의 텍스트

// 생성자
// - 위젯 클래스 및 인스턴스 초기화
// by Team4 (yeoul)
AOutGamePlayerController::AOutGamePlayerController()
    : MainMenuWidgetClass(nullptr), MainMenuWidgetInstance(nullptr)
{
}

// BeginPlay Override
// - 레벨 이름 확인 후 메인 메뉴 레벨일 경우 UI 표시
// by Team4 (yeoul)
void AOutGamePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 레벨 이름 가져오기
    FString CurrentMapName = GetWorld()->GetMapName();
    // 게임 실행 시 메인메뉴레벨에서 해당 UI 먼저 표시
    if (CurrentMapName.Contains("MainMenuLevel"))
    {
        ShowMainMenu(false);
    }
}

// 메인 메뉴 위젯 표시
// - 기존 위젯 제거 후 새로 생성
// - 입력 모드는 UIOnly로 설정 & 마우스 커서 표시
// - Start 버튼 텍스트는 bIsRestart 값에 따라 변경해서 Restart/Start 경우 분리
// by Team4 (yeoul)
void AOutGamePlayerController::ShowMainMenu(bool bIsRestart)
{
    // HUD 합치고 주석 해제하기
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

        // Start, Restart 텍스트 변경하기
        if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("Start_TEXT"))))
        {
            if (bIsRestart)
            {
                ButtonText->SetText(FText::FromString(TEXT("Restart")));
            }
            else
            {
                ButtonText->SetText(FText::FromString(TEXT("Start")));
            }
        }
    }
}


// 게임 HUD 표시

// 게임 시작 처리
// - 이후에 GameInstance 리셋 로직 추가 예정!
// - 현재는 MainMenuLevel 로드만 실행 (07-31)
// by Team4 (yeoul)
void AOutGamePlayerController::StartGame()
{
    // GameInstance 데이터 리셋 부분 나중에 추가
    // InGame 레벨로 이동만 시키기
    UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuLevel"));
}
