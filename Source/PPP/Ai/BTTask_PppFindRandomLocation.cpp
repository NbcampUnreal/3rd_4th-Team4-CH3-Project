#include "BTTask_PppFindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h" // UNavigationSystemV1 사용을 위해 필요

UBTTask_PppFindRandomLocation::UBTTask_PppFindRandomLocation()
{
    NodeName = TEXT("Find Random Location");
    SearchRadius = 1500.0f; // 기본 검색 반경 (15미터)

    bCreateNodeInstance = true;

    BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_PppFindRandomLocation, BlackboardKey));
}

EBTNodeResult::Type UBTTask_PppFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!AIController || !BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
    {
        return EBTNodeResult::Failed;
    }

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
    {
        return EBTNodeResult::Failed;
    }

    FNavLocation RandomLocation;

    // AI의 현재 위치를 중심으로 SearchRadius 반경 내에서 랜덤하게 도달 가능한 지점을 찾음
    bool bFound = NavSys->GetRandomReachablePointInRadius(AIPawn->GetActorLocation(), SearchRadius, RandomLocation);

    if (bFound)
    {
        // 유효한 랜덤 위치를 찾았으면 블랙보드에 저장하고 성공 반환
        BlackboardComp->SetValueAsVector(BlackboardKey.SelectedKeyName, RandomLocation.Location);
        return EBTNodeResult::Succeeded;
    }

    // NavMesh에서 유효한 위치를 찾지 못했으면 실패 반환
    return EBTNodeResult::Failed;
}
