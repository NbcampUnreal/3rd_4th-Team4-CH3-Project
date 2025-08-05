#include "BTDecorator_IsPlayerInMeleeRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UBTDecorator_IsPlayerInMeleeRange::UBTDecorator_IsPlayerInMeleeRange()
{
    NodeName = "Is Player in Melee Range";
}

bool UBTDecorator_IsPlayerInMeleeRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // 블랙보드 컴포넌트 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return false;

    // 플레이어 위치(Blackboard Key) 가져오기
    const FVector PlayerLocation = BlackboardComp->GetValueAsVector(BlackboardKey.SelectedKeyName);
    if (PlayerLocation == FVector::ZeroVector) return false;

    // AI 자신의 위치 가져오기
    const APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!AIPawn) return false;
    const FVector AILocation = AIPawn->GetActorLocation();

    // 플레이어와 AI 사이의 거리 계산
    const float DistanceSquared = FVector::DistSquared(AILocation, PlayerLocation);

    const float MeleeAttackRangeSquared = MeleeAttackRange * MeleeAttackRange;

    return DistanceSquared <= MeleeAttackRangeSquared;
}
