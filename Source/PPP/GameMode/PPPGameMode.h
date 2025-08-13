#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DummyEnemy.h"
#include "GameDefines.h"
#include "PPPGameState.h" // GameState 클래스 참조 추가
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

    // 여우리
    // 라운드 클리어 직후 출구 제한시간 시작 (시간 내 못 들어가면 GameOver)
    UFUNCTION(BlueprintCallable, Category="Stage")
    void StartExitWindow();

    // 코드 추가한 범인은 바로.... <김여울>
    // 게이트 겹침 시 호출(트리거나 BP에서 호출)
    UFUNCTION(BlueprintCallable, Category="Gate")
    void EnterNextStage();

    // 김여울
    // 출구 제한시간 초과 시(GameState에서 콜백)
    UFUNCTION()
    void OnExitTimeOver();

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

    // 김여울
    // 출구 제한시간 기본값(클리어 후 StartExitWindow 호출 시 쓰는 값)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gate|Timer", meta=(ClampMin="1.0"))
    float ExitWindowSeconds = 30.f;

    // 태그로 씬의 게이트 찾기(BP 트리거나 액터에 "StageGate" 태그 부여)
    UPROPERTY(EditAnywhere, Category="Gate")
    FName ExitGateTag = TEXT("StageGate");

    // 씬의 게이트 참조(자동 캐싱)
    UPROPERTY(VisibleAnywhere, Category="Gate")
    AActor* ExitGate = nullptr;

    // 출구 오픈 상태
    UPROPERTY(VisibleAnywhere, Category="Gate")
    bool bGateOpen = false;
};
