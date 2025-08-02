#include "TestEnemyKillQuest.h"


UTestEnemyKillQuest::UTestEnemyKillQuest()
    : TargetKillCount(0)
    , CurrentKillCount(0)
    , QuestState(EQuestState::NotStarted)
{
    QuestName = FText::FromString(TEXT("Default Kill Quest"));
    QuestDescription = FText::FromString(TEXT("Defeat enemies."));
}

bool UTestEnemyKillQuest::AddKillCount(int32 KillAmount)
{
    if (QuestState == EQuestState::InProgress)
    {
 
        CurrentKillCount = FMath::Min(CurrentKillCount + KillAmount, TargetKillCount);
        UE_LOG(LogTemp, Log, TEXT("Quest: %s - Current Kills: %d / %d"), *QuestName.ToString(), CurrentKillCount, TargetKillCount);

        if (CurrentKillCount >= TargetKillCount)
        {
            CompleteQuest();
            return true;
        }
    }
    return false;
}

void UTestEnemyKillQuest::AdvanceQuest(int32 NewTargetKillCount)
{
    TargetKillCount = NewTargetKillCount;
    CurrentKillCount = 0; // 다음 단계로 넘어가면 현재 처치 수는 0부터 다시 시작
    QuestState = EQuestState::InProgress;
    UE_LOG(LogTemp, Log, TEXT("Quest advanced! New Target: %d"), TargetKillCount);
}

void UTestEnemyKillQuest::CompleteQuest()
{
    QuestState = EQuestState::Completed;
    UE_LOG(LogTemp, Log, TEXT("Quest '%s' Completed!"), *QuestName.ToString());
}

void UTestEnemyKillQuest::ResetQuest()
{
    TargetKillCount = 0;
    CurrentKillCount = 0;
    QuestState = EQuestState::NotStarted;
    UE_LOG(LogTemp, Log, TEXT("Quest '%s' Reset."), *QuestName.ToString());
}
