#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "BTTask_ChaseTarget.h"
#include "Navigation/PathFollowingComponent.h" // UPathFollowingComponent를 위해 필요
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PPPAICharacter.h"
#include "PppFleeAICharacter.h"

UBTTask_ChaseTarget::UBTTask_ChaseTarget()
{
    NodeName = TEXT("Chase Target");
    bNotifyTick = true; // TickTask를 활성화
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ChaseTarget, TargetActorKey), AActor::StaticClass());

    AcceptanceRadius = 100.0f; // 기본값 설정 (AI가 목표에 도달했다고 판단할 최소 거리)
}

EBTNodeResult::Type UBTTask_ChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* MyController = OwnerComp.GetAIOwner();
    APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;
    UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

    if (!MyPawn || !MyBlackboard)
    {
        UE_LOG(LogTemp, Error, TEXT("BTTask_ChaseTarget: MyPawn or MyBlackboard is null. Task Failed."));
        return EBTNodeResult::Failed;
    }

    AActor* TargetActor = Cast<AActor>(MyBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("BTTask_ChaseTarget: TargetActor is null in Blackboard. Task Failed."));
        return EBTNodeResult::Failed;
    }

    // AI 캐릭터의 이동 속도 설정 (추격 속도로)
    if (APPPAICharacter* AIChar = Cast<APPPAICharacter>(MyPawn))
    {
        AIChar->SetMovementSpeed(AIChar->RunSpeed);
        UE_LOG(LogTemp, Warning, TEXT("BTTask_ChaseTarget: Setting speed to RunSpeed (%.1f)."), AIChar->RunSpeed);
    }
    else if (APppFleeAICharacter* FleeAIChar = Cast<APppFleeAICharacter>(MyPawn))
    {

    }


    // 이동 요청을 보냄
    EPathFollowingRequestResult::Type MoveResult = MyController->MoveToActor(TargetActor, AcceptanceRadius);

    if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_ChaseTarget: MoveToActor request successful. Task InProgress."));
        return EBTNodeResult::InProgress; // 이동이 끝날 때까지 TickTask에서 기다림
    }
    else if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        // 이미 목표 지점에 있다면 즉시 성공으로 처리
        if (APPPAICharacter* AIChar = Cast<APPPAICharacter>(MyPawn))
        {
            AIChar->SetMovementSpeed(AIChar->WalkSpeed); // 이동 완료 후 기본 속도로 변경
        }
        UE_LOG(LogTemp, Warning, TEXT("BTTask_ChaseTarget: Already at goal. Task Succeeded."));
        return EBTNodeResult::Succeeded;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BTTask_ChaseTarget: MoveToActor request failed (%d). Task Failed."), (int32)MoveResult);
        return EBTNodeResult::Failed;
    }
}

void UBTTask_ChaseTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* MyController = OwnerComp.GetAIOwner();
    APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;
    UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

    if (!MyPawn || !MyBlackboard)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* TargetActor = Cast<AActor>(MyBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // AI의 현재 위치와 목표(TargetActor)의 위치를 가져옴
    FVector AI_Location = MyPawn->GetActorLocation();
    FVector Target_Location = TargetActor->GetActorLocation();

    // 두 위치 간의 거리를 계산 Z축은 무시하여 평면 거리만 계산
    float DistanceToTarget = FVector::Dist2D(AI_Location, Target_Location);

    // AI 컨트롤러의 PathFollowingComponent를 가져옴
    UPathFollowingComponent* PathFollowingComp = MyController->GetPathFollowingComponent();

    // 1. 목표에 도달했는지 확인 (거리 기반)
    if (DistanceToTarget <= AcceptanceRadius)
    {
        // 목표에 도달
        if (APPPAICharacter* AIChar = Cast<APPPAICharacter>(MyPawn))
        {
            AIChar->SetMovementSpeed(AIChar->WalkSpeed); // 이동 완료 후 기본 속도로 변경
        }
        MyController->StopMovement(); // 혹시 모를 잔여 이동 명령 중지
        UE_LOG(LogTemp, Warning, TEXT("BTTask_ChaseTarget: Target reached, task succeeded. Distance: %.1f"), DistanceToTarget);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
    // 2. PathFollowingComponent가 유효한지 확인 후 이동 실패/경로 문제 체크
    else if (PathFollowingComp)
    {
        EPathFollowingStatus::Type MoveStatus = MyController->GetMoveStatus(); // AIController의 GetMoveStatus() 사용

        if (MoveStatus == EPathFollowingStatus::Idle || MoveStatus == EPathFollowingStatus::Waiting || MoveStatus == EPathFollowingStatus::Paused)
        {
            if (DistanceToTarget > AcceptanceRadius + 50.0f) // 약간의 여유를 주어 오차 허용
            {
                if (APPPAICharacter* AIChar = Cast<APPPAICharacter>(MyPawn))
                {
                    AIChar->SetMovementSpeed(AIChar->WalkSpeed);
                }
                UE_LOG(LogTemp, Error, TEXT("BTTask_ChaseTarget: AI stuck or failed to find path. Task Failed. Distance: %.1f, Status: %d"), DistanceToTarget, (int32)MoveStatus);
                FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
            }
        }
    }
    else
    {
        if (APPPAICharacter* AIChar = Cast<APPPAICharacter>(MyPawn))
        {
            AIChar->SetMovementSpeed(AIChar->WalkSpeed);
        }
        UE_LOG(LogTemp, Error, TEXT("BTTask_ChaseTarget: PathFollowingComponent is null. Task Failed."));
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
}
