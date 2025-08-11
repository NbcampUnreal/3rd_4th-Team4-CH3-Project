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

    // 스폰할 AI 클래스 목록 (BaseAICharacter 기반)
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TArray<TSubclassOf<APppBaseAICharacter>> EnemyClasses;

    // 클래스별 스폰 확률
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TArray<float> SpawnWeights;

    // 적 스폰 함수
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SpawnEnemies(int32 Count);

protected:
    UPROPERTY(VisibleAnywhere, Category = "Spawning")
    UBoxComponent* SpawnBox;

    FVector GetRandomPointInVolume() const;
    TSubclassOf<APppBaseAICharacter> GetRandomWeightedEnemyClass() const;

    virtual void BeginPlay() override;
};
