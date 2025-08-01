#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PppFleeAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class PPP_API APppFleeAIController : public AAIController
{
    GENERATED_BODY()

public:
    APppFleeAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

public:
    // 비헤이비어 트리 실행 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBehaviorTreeComponent* BehaviorTreeComponent;

    // 블랙보드 데이터를 관리하는 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;

    // 블랙보드 키 이름들
    static const FName PlayerLocationKey;
    static const FName PlayerDetectedKey;
    static const FName FleeLocationKey;

    UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }
};
