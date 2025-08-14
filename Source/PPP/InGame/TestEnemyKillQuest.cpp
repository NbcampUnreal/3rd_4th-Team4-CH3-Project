#include "TestEnemyKillQuest.h"

#include "PppCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EquipWeaponMaster.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h" // 시각화용 헤더 추가


UTestEnemyKillQuest::UTestEnemyKillQuest()
    : TargetKillCount(0)
    , CurrentKillCount(0)
    , QuestState(EQuestState::NotStarted)
    , OwnerActor(nullptr)
    , RewardWeaponClass(nullptr)
    , bRewardGiven(false)
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
    bRewardGiven = false; // ✅ 다음 퀘스트 진행시 보상 다시 가능하도록 초기화
    UE_LOG(LogTemp, Log, TEXT("Quest advanced! New Target: %d"), TargetKillCount);
}

void UTestEnemyKillQuest::CompleteQuest()
{
    QuestState = EQuestState::Completed;
    UE_LOG(LogTemp, Warning, TEXT("Quest '%s' Completed!"), *QuestName.ToString());

    if (bRewardGiven || !RewardWeaponClass || !OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Quest] 보상 스폰 조건 불충족 - bRewardGiven: %d, RewardWeaponClass: %s, OwnerActor: %s"),
            bRewardGiven ? 1 : 0,
            RewardWeaponClass ? *RewardWeaponClass->GetName() : TEXT("NULL"),
            OwnerActor ? *OwnerActor->GetName() : TEXT("NULL"));
        return;
    }

    FVector SpawnLocation = OwnerActor->GetActorLocation() + FVector(0, 0, 150.f);
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerActor;

    // 시각화
    DrawDebugSphere(OwnerActor->GetWorld(), SpawnLocation, 30.f, 12, FColor::Green, false, 5.f);

    // ✅ 올바른 타입으로 스폰
    APickUpWeaponMaster* Reward = OwnerActor->GetWorld()->SpawnActor<APickUpWeaponMaster>(
        RewardWeaponClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

    if (Reward)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Quest] 보상 무기 스폰 완료: %s"), *Reward->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Quest] 보상 스폰 실패"));
    }

    bRewardGiven = true;
}

void UTestEnemyKillQuest::ResetQuest()
{
    TargetKillCount = 0;
    CurrentKillCount = 0;
    QuestState = EQuestState::NotStarted;
    bRewardGiven = false; // ✅ 리셋 시 보상 플래그도 초기화
    UE_LOG(LogTemp, Log, TEXT("Quest '%s' Reset."), *QuestName.ToString());
}

void UTestEnemyKillQuest::SpawnRewardIfNeeded()
{
    if (bRewardGiven || !RewardWeaponClass || !OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Quest] SpawnRewardIfNeeded 조건 미충족 - bRewardGiven: %d, RewardWeaponClass: %s, OwnerActor: %s"),
            bRewardGiven ? 1 : 0,
            RewardWeaponClass ? *RewardWeaponClass->GetName() : TEXT("NULL"),
            OwnerActor ? *OwnerActor->GetName() : TEXT("NULL"));
        return;
    }

    // ✅ 머리 위에 보상 무기 스폰
    FVector SpawnLocation = OwnerActor->GetActorLocation() + FVector(0.f, 0.f, 150.f);
    FRotator SpawnRotation = FRotator::ZeroRotator;

    // ✅ 디버그용 구체 시각화
    DrawDebugSphere(OwnerActor->GetWorld(), SpawnLocation, 30.f, 12, FColor::Red, false, 5.f);

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerActor;

    UWorld* World = OwnerActor->GetWorld();
    if (World)
    {
        AEquipWeaponMaster* Reward = World->SpawnActor<AEquipWeaponMaster>(
            RewardWeaponClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (Reward)
        {
            UE_LOG(LogTemp, Warning, TEXT("[Quest] 보상 무기 스폰 완료: %s"), *Reward->GetName());
            bRewardGiven = true;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Quest] SpawnRewardIfNeeded: 보상 스폰 실패"));
        }
    }
}
