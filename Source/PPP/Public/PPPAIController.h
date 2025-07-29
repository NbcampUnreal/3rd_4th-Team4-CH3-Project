#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "PPPAIController.generated.h"

// 전방선언
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class APPPAICharacter; // 추격 AI
class APPPFleeAICharacter; // 도망 AI
/**
 *
 */
UCLASS()
class PPP_API APPPAIController : public AAIController
{
	GENERATED_BODY()

public:
	APPPAIController();

protected:
	//감각 perception
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "AI")
	UAIPerceptionComponent* AIPerception;

	// 시야
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	// 감지 후 추적 로직
	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	// 플레이어 감지 시 호출될 함수
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	bool bIsChasing = false; // 추격 중인지 나타내는 플래그
	FTimerHandle ChaseTimer; // 추격 관련 타이머

	void StartChasing(AActor* Target); // 추격 시작
	void StopChasing(); // 추격 중지
	void UpdateChase(); // 추격 중 주기적으로 호출될 함수

	bool bIsFleeing = false; // 도망 중인지 나타내는 플래그
	FTimerHandle FleeTimer; // 도망 관련 타이머

	UPROPERTY(EditAnywhere, Category = "AI")
	float FleeDistance = 1500.0f; // 플레이어로부터 도망갈 최소 거리

	void StartFlee(AActor* Target); // 도망 시작
	void StopFlee(); // 도망 중지
	void UpdateFlee(); // 도망 중 주기적으로 호출될 함수

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
private:
	// 랜덤으로 움직이게 함
	void MoveToRandomLocation();
	// n초마다 움직이게 함
	FTimerHandle RandomMoveTimer;
	// 이동 반경
	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveRadius = 1000.0f;
};
