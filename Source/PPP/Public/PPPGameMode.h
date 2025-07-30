#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DummyEnemy.h"
#include "GameDefines.h"
#include "PPPGameState.h" // ✅ GameState 클래스 참조 추가
#include "PPPGameMode.generated.h"

UCLASS()
class PPP_API APPPGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APPPGameMode();

	virtual void BeginPlay() override;

	/** 게임 상태 전환 */
	void SetGameState(EGameState NewState);

	/** 라운드 시작 처리 */
	UFUNCTION(BlueprintCallable)
	void StartRound();

	/** 라운드 종료 처리 */
	UFUNCTION(BlueprintCallable)
	void EndRound();

	/** 플레이어 사망 처리 */
	UFUNCTION(BlueprintCallable)
	void OnPlayerDeath();

	/** 적 사망 처리 */
	void OnEnemyKilled();

	/** 적 스폰 */
	void SpawnEnemies();

	UFUNCTION(BlueprintCallable)
	int32 GetMaxRounds() const;
    /**점수 보상 조건 확인 */
    void CheckRewardCondition();
protected:
	/** 최대 라운드 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round", meta=(AllowPrivateAccess = "true"))
	int32 MaxRounds = 3;

	/** 라운드당 적 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round")
	int32 EnemiesPerRound = 5;

	/** 적 스폰용 클래스 */
	UPROPERTY(EditDefaultsOnly, Category="Enemy")
	TSubclassOf<ADummyEnemy> EnemyClass;

    // [1단계] 보상으로 떨어질 액터 클래스 (블루프린트에서 설정)
    UPROPERTY(EditAnywhere, Category = "Reward")
    TSubclassOf<AActor> RewardActorClass;

    // [1단계] 이미 보상을 줬는지 여부 (중복 스폰 방지용)
    bool bRewardGiven = false;
};
