#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ChaseDetectPlayer.generated.h"

UCLASS()
class PPP_API UBTService_ChaseDetectPlayer : public UBTService
{
	GENERATED_BODY()

public:
    UBTService_ChaseDetectPlayer();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector PlayerLocationKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector PlayerDetectedKey;
};
