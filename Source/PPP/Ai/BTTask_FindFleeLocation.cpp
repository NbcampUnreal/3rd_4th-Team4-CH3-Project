#include "BTTask_FindFleeLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h" // NavMesh 시스템 사용을 위함
#include "Kismet/GameplayStatics.h"

UBTTask_FindFleeLocation::UBTTask_FindFleeLocation()
{
    NodeName = TEXT("FindFleeLocation");
    // 블랙보드 키 필터 설정
    PlayerLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindFleeLocation, PlayerLocationKey));
    FleeLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindFleeLocation, FleeLocationKey));
    FleeDistance = 2000.0f; // 기본 도망 거리 (조절 가능)
}

EBTNodeResult::Type UBTTask_FindFleeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* OwningPawn = AIController ? AIController->GetPawn() : nullptr;
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!OwningPawn || !BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    // 블랙보드에서 플레이어 위치 가져오기
    FVector PlayerLoc = BlackboardComp->GetValueAsVector(PlayerLocationKey.SelectedKeyName);
    FVector CurrentAILoc = OwningPawn->GetActorLocation();

    // AI가 플레이어로부터 도망갈 방향 (플레이어 위치 -> AI 위치 벡터의 연장선)
    FVector DirectionFromPlayerToAI = (CurrentAILoc - PlayerLoc).GetSafeNormal();

    // 도망갈 목표 위치 계산 (현재 위치에서 플레이어 반대 방향으로 FleeDistance만큼 떨어진 곳)
    FVector TargetLocation = CurrentAILoc + (DirectionFromPlayerToAI * FleeDistance);

    // NavMesh 위에서 유효하고 도달 가능한 위치 찾기
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        FNavLocation NavLocation;
        if (NavSys->GetRandomReachablePointInRadius(TargetLocation, FleeDistance * 2.0f, NavLocation))
        {
            // 유효한 위치를 찾았으면 블랙보드에 저장하고 성공 반환
            BlackboardComp->SetValueAsVector(FleeLocationKey.SelectedKeyName, NavLocation.Location);
            return EBTNodeResult::Succeeded;
        }
    }

    // NavMesh에서 유효한 위치를 찾지 못했으면 실패 반환
    return EBTNodeResult::Failed;
}
