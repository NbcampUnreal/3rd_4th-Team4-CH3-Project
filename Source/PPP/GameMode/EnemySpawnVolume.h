#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "EnemySpawnVolume.generated.h"

/**
 * 몬스터 스폰 볼륨 클래스
 * - 박스 콜리전 내에서 적을 랜덤 위치에 확률 기반으로 스폰
 */
UCLASS()
class PPP_API AEnemySpawnVolume : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawnVolume();

    // 적 클래스 목록
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TArray<TSubclassOf<AActor>> EnemyClasses;

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

    // 가중치 기반 적 클래스 선택
    TSubclassOf<AActor> GetRandomWeightedEnemyClass() const;

    virtual void BeginPlay() override;
};
