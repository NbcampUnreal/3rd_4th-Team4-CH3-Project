#include "EnemySpawnVolume.h"
#include "GameDefines.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

AEnemySpawnVolume::AEnemySpawnVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
    SetRootComponent(SpawnBox);
}

FVector AEnemySpawnVolume::GetRandomPointInVolume() const
{
    FVector Origin = SpawnBox->Bounds.Origin;
    FVector Extent = SpawnBox->Bounds.BoxExtent;
    return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

TSubclassOf<APppBaseAICharacter> AEnemySpawnVolume::GetRandomWeightedEnemyClass() const
{
    if (EnemyClasses.Num() == 0 || SpawnWeights.Num() != EnemyClasses.Num())
    {
        UE_LOG(LogDebug, Error, TEXT("EnemyClasses 또는 SpawnWeights 설정 오류"));
        return nullptr;
    }

    float TotalWeight = 0.0f;
    for (float Weight : SpawnWeights) { TotalWeight += Weight; }

    if (TotalWeight <= 0.0f)
    {
        UE_LOG(LogDebug, Error, TEXT("SpawnWeights 총합이 0입니다."));
        return nullptr;
    }

    float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
    float AccumulatedWeight = 0.0f;

    for (int32 i = 0; i < EnemyClasses.Num(); ++i)
    {
        AccumulatedWeight += SpawnWeights[i];
        if (RandomValue <= AccumulatedWeight)
        {
            return EnemyClasses[i];
        }
    }

    return EnemyClasses.Last();
}

void AEnemySpawnVolume::SpawnEnemies(int32 Count)
{
    if (EnemyClasses.Num() == 0 || SpawnWeights.Num() != EnemyClasses.Num())
    {
        UE_LOG(LogDebug, Error, TEXT("EnemyClasses와 SpawnWeights 수가 일치하지 않습니다."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    int32 SpawnedCount = 0;

    for (int32 i = 0; i < Count; ++i)
    {
        TSubclassOf<APppBaseAICharacter> SelectedClass = GetRandomWeightedEnemyClass();
        if (!SelectedClass) continue;

        FVector SpawnLocation = GetRandomPointInVolume();
        FRotator SpawnRotation = FRotator::ZeroRotator;

        APppBaseAICharacter* SpawnedAI = World->SpawnActor<APppBaseAICharacter>(SelectedClass, SpawnLocation, SpawnRotation);
        if (SpawnedAI)
        {
            SpawnedAI->SpawnDefaultController();
            SpawnedCount++;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[SpawnEnemies] 요청: %d, 스폰됨: %d"), Count, SpawnedCount);
}

void AEnemySpawnVolume::TriggerSpawn(int32 CurrentFloor, int32 EnemyCount)
{
    if (FloorIndex == CurrentFloor)
    {
        UE_LOG(LogTemp, Warning, TEXT("스폰볼륨 [%s]: 현재 층과 일치. 적 스폰 시도"), *GetName());
        SpawnEnemies(EnemyCount);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("스폰볼륨 [%s]: 현재 층과 불일치. 무시됨 (FloorIndex=%d, CurrentFloor=%d)"),
               *GetName(), FloorIndex, CurrentFloor);
    }
}

void AEnemySpawnVolume::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("SpawnVolume BeginPlay 호출됨"));
    SpawnEnemies(0);
}
