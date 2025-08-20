#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_IsPlayerInMeleeRange.generated.h"

UCLASS()
class PPP_API UBTDecorator_IsPlayerInMeleeRange : public UBTDecorator_Blackboard
{
    GENERATED_BODY()
public:
    UBTDecorator_IsPlayerInMeleeRange();

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    float MeleeAttackRange = 200.0f;
protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
