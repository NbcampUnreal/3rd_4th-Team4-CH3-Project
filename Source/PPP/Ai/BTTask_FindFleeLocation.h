#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindFleeLocation.generated.h"

UCLASS()
class PPP_API UBTTask_FindFleeLocation : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FindFleeLocation();

    // 태스크 실행 로직
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector PlayerLocationKey; // 플레이어 위치 키

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector FleeLocationKey;   // 도망갈 위치를 저장할 키

    UPROPERTY(EditAnywhere, Category = "AI")
    float FleeDistance; // 도망갈 거리
};
