// EnemySpawnVolume.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "EnemySpawnVolume.generated.h"

/**
 * 몬스터 스폰 볼륨 클래스
 * - 레벨에 배치 가능한 스폰 영역 액터
 * - Box 콜리전 내부에서 적을 랜덤한 위치에 스폰함
 * - GameMode에서 호출하여 웨이브 단위로 적 스폰 가능
 */
UCLASS()
class PPP_API AEnemySpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	// 생성자 - 기본 컴포넌트 초기화
	AEnemySpawnVolume();

	// 스폰할 적 클래스 (블루프린트에서 지정 가능)
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> EnemyClass;

	/**
	 * 지정한 개수만큼 적을 박스 내부에 스폰함
	 * @param Count 스폰할 적의 수
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnEnemies(int32 Count);

protected:
	// 박스 콜리전 컴포넌트 (스폰 영역)
	UPROPERTY(VisibleAnywhere, Category = "Spawning")
	UBoxComponent* SpawnBox;

	/**
	 * 박스 영역 내의 랜덤 위치를 반환
	 * @return 박스 내부의 랜덤 위치
	 */
	FVector GetRandomPointInVolume() const;
};
