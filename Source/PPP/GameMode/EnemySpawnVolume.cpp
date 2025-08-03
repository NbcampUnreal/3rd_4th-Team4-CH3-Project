// EnemySpawnVolume.cpp

#include "EnemySpawnVolume.h"

#include "GameDefines.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

// 생성자 - Box 컴포넌트 생성 및 루트로 설정
AEnemySpawnVolume::AEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// 박스 컴포넌트 생성 및 루트 설정
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SetRootComponent(SpawnBox);
}

// 박스 영역 내에서 랜덤 위치 반환
FVector AEnemySpawnVolume::GetRandomPointInVolume() const
{
	FVector Origin = SpawnBox->Bounds.Origin;
	FVector Extent = SpawnBox->Bounds.BoxExtent;

	// UKismetMathLibrary를 사용해 박스 내부의 무작위 위치 계산
	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

// 지정된 수만큼 적을 스폰함
void AEnemySpawnVolume::SpawnEnemies(int32 Count)
{
	if (!EnemyClass)
	{
		UE_LOG(LogDebug, Error, TEXT("EnemyClass가 설정되지 않았습니다."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < Count; ++i)
	{
		FVector SpawnLocation = GetRandomPointInVolume();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		// 적 스폰
		World->SpawnActor<AActor>(EnemyClass, SpawnLocation, SpawnRotation);
	}
}
