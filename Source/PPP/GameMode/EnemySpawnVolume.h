#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "../AI/PppBaseAICharacter.h" // Base AI Character
#include "EnemySpawnVolume.generated.h"

UCLASS()
class PPP_API AEnemySpawnVolume : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawnVolume();
    // 스폰볼륨이 몇 라운드(몇 층)에 해당하는지 설정하는 값
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    int32 RoundIndexToSpawn = 1; // 예: 1층이면 1

    // 스폰할 AI 클래스 목록 (BaseAICharacter 기반)
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TArray<TSubclassOf<APppBaseAICharacter>> EnemyClasses;

    // 클래스별 스폰 확률
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TArray<float> SpawnWeights;

    // 적 스폰 함수
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SpawnEnemies(int32 Count);

    UPROPERTY(EditAnywhere, Category="Spawning")
    int32 FloorIndex = 1; // 이 볼륨이 속한 층 (예: 1층, 2층, 3층 등)

    UFUNCTION(BlueprintCallable, Category="Spawning")
    void TriggerSpawn(int32 CurrentFloor, int32 EnemyCount);

protected:
    UPROPERTY(VisibleAnywhere, Category = "Spawning")
    UBoxComponent* SpawnBox;

    FVector GetRandomPointInVolume() const;
    TSubclassOf<APppBaseAICharacter> GetRandomWeightedEnemyClass() const;

    virtual void BeginPlay() override;
};
