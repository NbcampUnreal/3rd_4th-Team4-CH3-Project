#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DummyEnemy.h"
#include "GameDefines.h"
#include "PPPGameState.h" // ✅ GameState 클래스 참조 추가
#include "../Ai/PppBaseAICharacter.h" // Base AI Character
#include "Blueprint/UserWidget.h" // 정현성 타임 UI 추가
#include "PPPGameMode.generated.h"

UCLASS()
class PPP_API APPPGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APPPGameMode();

	virtual void BeginPlay() override;

    // 현재 라운드 번호 반환
    UFUNCTION(BlueprintCallable, Category="Round")
    int32 GetCurrentRound() const { return CurrentRound; }

    // 현재 라운드 진행 여부 반환
    UFUNCTION(BlueprintCallable, Category="Round")
    bool IsRoundActive() const { return bRoundActive; }

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

    void OnRoundCleared(); //라운드 성공
    UFUNCTION()
    void OnGameOver(); // 게임 오버

	UFUNCTION(BlueprintCallable)
	int32 GetMaxRounds() const;
    /**점수 보상 조건 확인 */
    void CheckRewardCondition();

    /**stage(라운드) 관리 */
    UFUNCTION(BlueprintPure, Category="Round")
    int32 GetCurrentFloor() const { return CurrentFloor; }

    UFUNCTION(BlueprintCallable, Category="Round")
    void SetCurrentFloor(int32 NewFloor) { CurrentFloor = NewFloor; }

    // 라운드 클리어 시 다음 층으로 이동 가능 신호(디자이너가 바인딩)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundClearedSignal);

    UPROPERTY(BlueprintAssignable, Category="Round")
    FOnRoundClearedSignal OnRoundClearedSignal;
protected:

    // 현재 라운드 번호
    UPROPERTY(VisibleAnywhere, Category="Round")
    int32 CurrentRound = 1;

    // 라운드 진행 중 여부
    UPROPERTY(VisibleAnywhere, Category="Round")
    bool bRoundActive = false;

    /**stage1 층(라운드) 관리 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round")
    int32 CurrentFloor = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round")
    int32 MaxFloors = 3;

	/** 최대 라운드 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round", meta=(AllowPrivateAccess = "true"))
	int32 MaxRounds = 3;

	/** 라운드당 적 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round")
	int32 EnemiesPerRound = 5;

	// 테스트용이라 주석처리
	// UPROPERTY(EditDefaultsOnly, Category="Enemy")
	// TSubclassOf<ADummyEnemy> EnemyClass;

    // [1단계] 보상으로 떨어질 액터 클래스 (블루프린트에서 설정)
    UPROPERTY(EditAnywhere, Category = "Reward")
    TSubclassOf<AActor> RewardActorClass;
    /** 적 스폰용 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    TSubclassOf<APppBaseAICharacter> EnemyClass;

    // [1단계] 이미 보상을 줬는지 여부 (중복 스폰 방지용)
    bool bRewardGiven = false;

    // [추가] 스테이지 전용 타이머 사용/지속시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timer")
    bool bUseStageTimer = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timer", meta=(ClampMin="1.0"))
    float StageTimerSeconds = 120.f;

    // 정현성
    // 타임 위젯 블루프린트 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> TimeWidgetClass;
};
