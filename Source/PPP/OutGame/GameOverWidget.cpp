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

    if (UPPPGameInstance* GI = GetGameInstance<UPPPGameInstance>())
    {
        int32 FinalScore = GI->FinalScore;
        UE_LOG(LogTemp, Warning, TEXT("[Widget] NativeConstruct에서 GI->FinalScore = %d"), FinalScore);

        if (ScoreText)
        {
            ScoreText->SetText(FText::AsNumber(FinalScore));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Widget] ScoreText is nullptr! 바인딩 확인 필요"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Widget] GameInstance 캐스팅 실패"));
    }
}


void UGameOverWidget::NativeDestruct()
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
    UE_LOG(LogTemp, Warning, TEXT("[Widget] 최종 점수 표시: %d"), FinalScore);

    if (ScoreText)
    {
        FString ScoreString = FString::Printf(TEXT("Total Score : %d"), FinalScore);
        ScoreText->SetText(FText::FromString(ScoreString));
    }
}

void UGameOverWidget::HandlePlayerDeath()
{}
