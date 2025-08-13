

#include "TestQuestActorComponent.h"
#include "Engine/World.h"
#include "TestEnemyKillQuest.h"


UTestQuestActorComponent::UTestQuestActorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 퀘스트 단계 기본값 설정 (에디터에서 변경 가능)
    QuestStages.Add(20);
    QuestStages.Add(30);
    QuestStages.Add(40);

    CurrentQuestStageIndex = -1; // 퀘스트 시작 전
}


// 게임 시작 시 호출
void UTestQuestActorComponent::BeginPlay()
{
    Super::BeginPlay();


}


void UTestQuestActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 필요하다면 여기에 Tick 로직을 추가
}

void UTestQuestActorComponent::StartQuest()
{

    if (QuestStages.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestStages 배열이 비어 있습니다. 에디터에서 퀘스트 단계를 설정해 주세요. 퀘스트를 시작할 수 없습니다."));
        return;
    }


    if (CurrentQuestStageIndex == -1) // 퀘스트가 처음 시작될 때
    {
        CurrentQuestStageIndex = 0;
        CurrentQuest = NewObject<UTestEnemyKillQuest>(this, UTestEnemyKillQuest::StaticClass(), TEXT("KillQuestInstance"));
        CurrentQuest->AdvanceQuest(QuestStages[CurrentQuestStageIndex]);
        CurrentQuest->QuestName = FText::Format(FText::FromString(TEXT("Enemy Kill Quest Stage {0}")), FText::AsNumber(CurrentQuestStageIndex + 1));
        CurrentQuest->QuestDescription = FText::Format(FText::FromString(TEXT("Defeat {0} enemies.")), FText::AsNumber(QuestStages[CurrentQuestStageIndex]));


        OnQuestProgressUpdated.Broadcast(CurrentQuest->CurrentKillCount, CurrentQuest->TargetKillCount);
        UE_LOG(LogTemp, Log, TEXT("퀘스트 시작: '%s' 목표: %d"), *CurrentQuest->QuestName.ToString(), CurrentQuest->TargetKillCount);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("퀘스트가 이미 시작되었거나 완료되었습니다."));
    }
}

void UTestQuestActorComponent::OnEnemyKilled(int32 KillAmount)
{

    if (!CurrentQuest || CurrentQuest->QuestState != EQuestState::InProgress)
    {
        UE_LOG(LogTemp, Warning, TEXT("활성화된 퀘스트가 없거나 퀘스트가 진행 중이 아닙니다."));
        return;
    }


    bool bQuestCompleted = CurrentQuest->AddKillCount(KillAmount);


    // OnQuestProgressUpdated.Broadcast(CurrentQuest->CurrentKillCount, CurrentQuest->TargetKillCount);

    if (bQuestCompleted)
    {
        UE_LOG(LogTemp, Log, TEXT("퀘스트 단계 완료: %d"), CurrentQuestStageIndex + 1);
        OnQuestStageCompleted.Broadcast(CurrentQuestStageIndex);
        GoToNextQuestStage(); // 다음 퀘스트 단계로 진행
    }
}

void UTestQuestActorComponent::GoToNextQuestStage()
{
    // 다음 퀘스트 단계가 있는지 확인
    if (CurrentQuestStageIndex + 1 < QuestStages.Num())
    {
        CurrentQuestStageIndex++;

        if (CurrentQuest)
        {
            CurrentQuest->AdvanceQuest(QuestStages[CurrentQuestStageIndex]);
            CurrentQuest->QuestName = FText::Format(FText::FromString(TEXT("Enemy Kill Quest Stage {0}")), FText::AsNumber(CurrentQuestStageIndex + 1));
            CurrentQuest->QuestDescription = FText::Format(FText::FromString(TEXT("Defeat {0} enemies.")), FText::AsNumber(QuestStages[CurrentQuestStageIndex]));
        }
        else
        {

            CurrentQuest = NewObject<UTestEnemyKillQuest>(this, UTestEnemyKillQuest::StaticClass(), TEXT("KillQuestInstance"));
            CurrentQuest->AdvanceQuest(QuestStages[CurrentQuestStageIndex]);
            CurrentQuest->QuestName = FText::Format(FText::FromString(TEXT("Enemy Kill Quest Stage {0}")), FText::AsNumber(CurrentQuestStageIndex + 1));
            CurrentQuest->QuestDescription = FText::Format(FText::FromString(TEXT("Defeat {0} enemies.")), FText::AsNumber(QuestStages[CurrentQuestStageIndex]));
        }
        UE_LOG(LogTemp, Log, TEXT("다음 퀘스트 단계로 이동: %d, 목표: %d"), CurrentQuestStageIndex + 1, CurrentQuest->TargetKillCount);
        OnQuestProgressUpdated.Broadcast(CurrentQuest->CurrentKillCount, CurrentQuest->TargetKillCount);
    }
    else
    {

        UE_LOG(LogTemp, Log, TEXT("모든 퀘스트 단계가 완료되었습니다!"));
        if (CurrentQuest)
        {
            CurrentQuest->CompleteQuest(); // 최종적으로 완료 상태로 설정
        }
        OnAllQuestsCompleted.Broadcast();
    }
}

bool UTestQuestActorComponent::AreAllQuestsCompleted() const
{
    return CurrentQuestStageIndex >= QuestStages.Num();
}
