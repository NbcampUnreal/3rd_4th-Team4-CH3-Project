// UBTService_ChaseDetectPlayer.h (수정)
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTService_ChaseDetectPlayer.generated.h"

UCLASS(Blueprintable)
class PPP_API UBTService_ChaseDetectPlayer : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_ChaseDetectPlayer();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    // 플레이어가 탐지되었는지 여부를 저장할 블랙보드 키
    UPROPERTY(EditAnywhere, Category = "AI")
    FBlackboardKeySelector PlayerDetectedKey;

    // 플레이어의 위치를 저장할 블랙보드 키
    UPROPERTY(EditAnywhere, Category = "AI")
    FBlackboardKeySelector PlayerLocationKey;
};
