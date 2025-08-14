#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h" // UTextBlock 사용을 위한 헤더 추가
#include "Kismet/GameplayStatics.h"
#include "../Characters/PppCharacter.h" // 추가
#include "PPP/GameMode/PPPGameMode.h"
#include "PPP/GameMode/PPPGameInstance.h"


void UGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Return_BTN)
    {
        Return_BTN->OnClicked.AddDynamic(this, &UGameOverWidget::OnReturnToMainMenuClicked);
    }

    // 게임 인스턴스에서 최종 점수 가져오기
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UPPPGameInstance* PPPGameInstance = Cast<UPPPGameInstance>(GameInstance))
        {
            SetFinalScore(PPPGameInstance->FinalScore);
        }
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
     // 레벨 이동 전 게임 일시정지 해제
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    UGameplayStatics::OpenLevel(this, FName("MainMenuLevel"));
}

void UGameOverWidget::SetFinalScore(int32 FinalScore)
{
    if (ScoreText)
    {
        FString ScoreString = FString::Printf(TEXT("Total Score : %d"), FinalScore);
        ScoreText->SetText(FText::FromString(ScoreString));
    }
}

void UGameOverWidget::HandlePlayerDeath()
{}
