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

// 가중치 기반 적 클래스 선택 함수
TSubclassOf<AActor> AEnemySpawnVolume::GetRandomWeightedEnemyClass() const
{
    if (EnemyClasses.Num() == 0 || SpawnWeights.Num() != EnemyClasses.Num())
    {
        UE_LOG(LogDebug, Error, TEXT("EnemyClasses 또는 SpawnWeights 설정 오류"));
        return nullptr;
    }

    float TotalWeight = 0.0f;
    for (float Weight : SpawnWeights)
    {
        TotalWeight += Weight;
    }

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

    // 예외 처리 (마지막 클래스)
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

    for (int32 i = 0; i < Count; ++i)
    {
        TSubclassOf<AActor> SelectedClass = GetRandomWeightedEnemyClass();
        if (!SelectedClass) continue;

        FVector SpawnLocation = GetRandomPointInVolume();
        FRotator SpawnRotation = FRotator::ZeroRotator;

        World->SpawnActor<AActor>(SelectedClass, SpawnLocation, SpawnRotation);
    }
}
void AEnemySpawnVolume::BeginPlay()
{
    Super::BeginPlay();

    // 디버깅용 로그
    UE_LOG(LogTemp, Warning, TEXT("SpawnVolume BeginPlay 호출됨"));

    // 원하는 수만큼 적 스폰 (임시로 3마리)
    SpawnEnemies(3);
}
