#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/PppPlayerController.h"

void UPauseMenuWidget::NativeConstruct()
{
    // 부모 함수 먼저 호출
    // 이 위젯이 만들어질 때 버튼을 눌렀을 때 호출할 이벤트 연결
    Super::NativeConstruct();

    if (Resume_BTN)
    {
        Resume_BTN->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
    }

    if (Return_BTN)
    {
        Return_BTN->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnReturnClicked);
    }
}

void UPauseMenuWidget::OnResumeClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Resume Clicked"));

    if (APppPlayerController* PC = Cast<APppPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->SetPause(false); // 게임 재개
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());

        this->RemoveFromParent(); // 정지 UI 닫기
    }
}


void UPauseMenuWidget::OnReturnClicked()
{

    if (APppPlayerController* PC = Cast<APppPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->ShowMainMenu(true); // MainMenu로 복귀
    }
    UE_LOG(LogTemp, Log, TEXT("Return Clicked"));
}
