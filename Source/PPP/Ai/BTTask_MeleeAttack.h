#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MeleeAttack.generated.h"


// 태스크 메모리 구조체: 태스크 인스턴스별로 상태를 저장
struct FBTMeleeAttackTaskMemory
{
    // 공격 중인지 여부를 저장하는 변수
    bool bIsAttacking;
};

UCLASS()
class PPP_API UBTTask_MeleeAttack : public UBTTask_BlackboardBase
{
    GENERATED_BODY()
public:
    // 생성자
    UBTTask_MeleeAttack();

    // 태스크가 실행될 때 호출됨
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    // 태스크가 InProgress 상태일 때 매 프레임 호출됨
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // 태스크의 메모리 크기를 반환 (NodeMemory 사용을 위해 필요)
    virtual uint16 GetInstanceMemorySize() const override;
};
