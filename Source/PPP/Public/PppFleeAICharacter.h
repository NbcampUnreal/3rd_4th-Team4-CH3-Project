#pragma once

#include "CoreMinimal.h"
#include "PppBaseAICharacter.h"
#include "PppFleeAICharacter.generated.h"

class UAIPerceptionComponent; // 전방 선언: AI Perception Component
class UAISenseConfig_Sight;   // 전방 선언: 시야 센서 설정 클래스

UCLASS()
class PPP_API APppFleeAICharacter : public APppBaseAICharacter
{
    GENERATED_BODY()

public:
    APppFleeAICharacter();

protected:
    virtual void BeginPlay() override;

public:
    // AI Perception Component 추가
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* PerceptionComponent;

    // 시야 센서 설정
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAISenseConfig_Sight* SightConfig;

    // 플레이어 감지 시 또는 감지 잃을 시 호출될 함수
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    // 기존 변수들 유지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Flee")
    float MinFleeDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Flee")
    float MaxFleeDistance;


};
