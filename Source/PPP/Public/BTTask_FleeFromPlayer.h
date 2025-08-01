/*
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BTTask_FleeFromPlayer.generated.h"

UCLASS()
class PPP_API UBTTask_FleeFromPlayer : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FleeFromPlayer();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    // 도망칠 대상(플레이어) 액터를 가져올 블랙보드 키
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

    // 플레이어로부터 얼마나 멀리 도망칠지 결정하는 거리
    UPROPERTY(EditAnywhere, Category = "AI")
    float FleeDistance = 2500.0f; // APPPAIController의 FleeDistance와 동일하게 설정
};
*/
