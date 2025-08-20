#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_SetIsInvestigatingFalse.generated.h"


UCLASS()
class PPP_API UBTS_SetIsInvestigatingFalse : public UBTService_BlackboardBase
{
    GENERATED_BODY()

public:
    UBTS_SetIsInvestigatingFalse();

protected:
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


};
