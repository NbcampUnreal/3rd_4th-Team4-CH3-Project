/*
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "BTTask_FleeFromPlayer.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PppFleeAICharacter.h" // 도망 AI 캐릭터 클래스 헤더 (SetMovementSpeed 사용)
// PathFollowingComponent.h의 정확한 경로를 포함해야 합니다.
#include "Navigation/PathFollowingComponent.h" // <--- 이 줄을 추가합니다.

UBTTask_FleeFromPlayer::UBTTask_FleeFromPlayer()
{
    NodeName = TEXT("Flee From Player");
    bNotifyTick = true; // 이동 완료를 감지하려면 TickTask 활성화

    // TargetActorKey가 Object 타입의 블랙보드 키만 선택할 수 있도록 필터를 추가합니다.
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FleeFromPlayer, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_FleeFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* MyController = OwnerComp.GetAIOwner();
    APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;
    UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

    if (!MyPawn || !MyBlackboard)
    {
        return EBTNodeResult::Failed;
    }

    AActor* TargetActor = Cast<AActor>(MyBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        return EBTNodeResult::Failed;
    }

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(MyPawn->GetWorld());
    if (!NavSystem)
    {
        return EBTNodeResult::Failed;
    }

    // 도망 AI 캐릭터의 이동 속도를 RunSpeed로 설정합니다.
    if (APppFleeAICharacter* AIChar = Cast<APppFleeAICharacter>(MyPawn))
    {
        AIChar->SetMovementSpeed(AIChar->RunSpeed);
    }

    // 플레이어로부터 멀어지는 방향을 계산합니다.
    FVector FleeDirection = (MyPawn->GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal();
    FVector CandidateFleeLocation = MyPawn->GetActorLocation() + FleeDirection * FleeDistance;

    FNavLocation FleeNavLocation;
    bool bFoundLocation = NavSystem->GetRandomPointInNavigableRadius(
        CandidateFleeLocation,
        FleeDistance, // 도망갈 위치를 찾을 반경도 FleeDistance로 설정하여 플레이어로부터 멀리 떨어진 지점을 찾도록 합니다.
        FleeNavLocation
    );

    if (bFoundLocation)
    {
        // EPathFollowingRequestResult::Type과 멤버 이름은 PathFollowingComponent.h에 정확히 정의되어 있습니다.
        EPathFollowingRequestResult::Type MoveResult = MyController->MoveToLocation(FleeNavLocation.Location);

        if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal || MoveResult == EPathFollowingRequestResult::RequestSuccessful)
        {
            return EBTNodeResult::InProgress;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BTTask: Flee MoveToLocation request failed."));
            return EBTNodeResult::Failed;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BTTask: No navigable location found for fleeing."));
        return EBTNodeResult::Failed;
    }
}

void UBTTask_FleeFromPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* MyController = OwnerComp.GetAIOwner();
    if (MyController)
    {
        EPathFollowingStatus::Type MoveStatus = MyController->GetMoveStatus();

        // PathFollowingComponent.h에서 확인된 EPathFollowingStatus 정의를 사용합니다.
        // EPathFollowingStatus에는 Aborted가 없습니다.
        if (MoveStatus == EPathFollowingStatus::Idle) // 이동 완료 (목표에 도달하여 멈춤)
        {
            // 도망 후 캐릭터 속도를 다시 WalkSpeed로 되돌립니다.
            if (APppFleeAICharacter* AIChar = Cast<APppFleeAICharacter>(MyController->GetPawn()))
            {
                AIChar->SetMovementSpeed(AIChar->WalkSpeed);
            }
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 성공으로 태스크 종료
        }
        else if (MoveStatus == EPathFollowingStatus::Moving || MoveStatus == EPathFollowingStatus::Waiting || MoveStatus == EPathFollowingStatus::Paused)
        {
            // 이동 중이거나, 경로를 기다리거나, 일시 정지된 상태이므로 다음 틱을 기다립니다.
        }
        else // 그 외의 모든 상태 (예상치 못한 오류, 경로 추적 중단 등)
        {
            // 이 Branch는 MoveStatus가 EPathFollowingStatus에 정의된 Enum이 아닌 경우에만 도달합니다.
            // UE_LOG(LogTemp, Warning, TEXT("BTTask_FleeFromPlayer: Unexpected PathFollowing Status encountered: %s"), *UEnum::GetValueAsString(TEXT("EPathFollowingStatus"), (int32)MoveStatus));

            // 도망 중단 후 캐릭터 속도를 WalkSpeed로 되돌립니다.
            if (APppFleeAICharacter* AIChar = Cast<APppFleeAICharacter>(MyController->GetPawn()))
            {
                AIChar->SetMovementSpeed(AIChar->WalkSpeed);
            }
            FinishLatentTask(OwnerComp, EBTNodeResult::Failed); // 실패로 태스크 종료
        }
    }
}
*/
