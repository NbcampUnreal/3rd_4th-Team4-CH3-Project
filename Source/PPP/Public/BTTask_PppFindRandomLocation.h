#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BTTask_PppFindRandomLocation.generated.h"



UCLASS()
class PPP_API UBTTask_PppFindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
    UBTTask_PppFindRandomLocation();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    // 블랙보드 키 선택기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
    float SearchRadius;


protected:
};
