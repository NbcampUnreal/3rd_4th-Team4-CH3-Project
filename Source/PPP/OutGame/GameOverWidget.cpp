#include "GameOverWidget.h"
#include "Components/Button.h"
#include "../Characters/PppCharacter.h"
#include "Kismet/GameplayStatics.h"

void UGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Return_BTN)
    {
        Return_BTN->OnClicked.AddDynamic(this, &UGameOverWidget::OnReturnToMainMenuClicked);
    }

    // 캐릭터의 델리게이트를 바인딩해서 로그 찍어보기 -> GameMode에서 바인딩되면 이부분 삭제
    /*
    if (APppCharacter* PlayerChar = Cast<APppCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        PlayerChar->OnCharacterDead.AddDynamic(this, &UGameOverWidget::HandlePlayerDeath);
    }
    */
}

void UGameOverWidget::OnReturnToMainMenuClicked()
{
    // MainMenuLevel로 돌아가기
    UGameplayStatics::OpenLevel(this, FName("MainMenuLevel"));
    UE_LOG(LogTemp, Log, TEXT("Return to Main Menu Clicked"));
}

void UGameOverWidget::HandlePlayerDeath()
{
    UE_LOG(LogTemp, Warning, TEXT("GameOverWidget detected: Player is Dead."));
}
