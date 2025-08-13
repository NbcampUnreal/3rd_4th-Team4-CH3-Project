#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
// Perception 관련 헤더는 필요 시 포함합니다.
#include "Perception/AIPerceptionTypes.h" // FAIStimulus
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "PPPAIController.generated.h"

// 필요한 클래스 전방 선언 (TObjectPtr 사용 시 필수 아님, 하지만 명시적으로 두면 좋음)
class UBehaviorTree;
class UBlackboardData;
// UBlackboardComponent는 AAIController에 이미 내장되어 있으므로 전방 선언 불필요

UCLASS()
class PPP_API APPPAIController : public AAIController
{
    GENERATED_BODY()

public:
    APPPAIController();

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override; // OnPossess에서 BT 실행

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
    float PeripheralVisionAngleDegrees;

    // 비헤이비어 트리 에셋을 레퍼런싱하기 위한 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

    // 블랙보드 데이터 에셋을 레퍼런싱하기 위한 변수 (비헤이비어 트리에 종속될 수 있음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TObjectPtr<UBlackboardData> BlackboardDataAsset; // 필요하다면 별도로 명시

    // AI Perception Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

    // 시야 센스 설정
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<UAISenseConfig_Sight> SightConfig;

    // AI 특성 설정 변수 (블루프린트에서 편집 가능)
    UPROPERTY(EditAnywhere, Category = "AI|Settings")
    float MoveRadius = 1000.0f;

    UPROPERTY(EditAnywhere, Category = "AI|Sight Settings - Chase")
    float ChaseSightRadius = 1500.0f;
    UPROPERTY(EditAnywhere, Category = "AI|Sight Settings - Chase")
    float ChaseLoseSightRadius = 2000.0f;
    UPROPERTY(EditAnywhere, Category = "AI|Sight Settings - Chase")
    float ChasePeripheralVisionAngleDegrees = 90.0f;
    UPROPERTY(EditAnywhere, Category = "AI|Sight Settings - Chase")
    float ChaseMaxAge = 5.0f;

    UPROPERTY(EditAnywhere, Category = "AI|Sight Settings - Flee")
    float FleeSightRadius = 1000.0f;
    UPROPERTY(EditAnywhere, Category = "AI|Sight Settings - Flee")
    float FleeLoseSightRadius = 1500.0f;
    UPROPERTY(EditAnywhere, Category = "AI|Sight Settings - Flee")
    float FleePeripheralVisionAngleDegrees = 160.0f;
    UPROPERTY(EditAnywhere, Category = "AI|Sight Settings - Flee")
    float FleeMaxAge = 30.0f;

    UPROPERTY(EditAnywhere, Category = "AI|Flee")
    float FleeDistance = 2500.0f;

private:
    // AI Perception Component의 콜백 함수
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus); // 이름 변경 (충돌 방지 및 명확성)

public:
    // 블랙보드 키 이름 (FName으로 정의)
    // static const FName으로 선언하여 .cpp 파일에서 초기화하는 것이 좋습니다.
    // 이렇게 해야 모든 AIController 인스턴스가 동일한 FName을 공유합니다.
    static const FName TargetActorKeyName;
    static const FName CanSeeTargetKeyName;
    static const FName TargetLastKnownLocationKeyName;
    static const FName IsInvestigatingKeyName;
    static const FName PatrolLocationKeyName; // 추가된 패트롤 위치 키
};
