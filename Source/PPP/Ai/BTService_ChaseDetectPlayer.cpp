#include "BTService_ChaseDetectPlayer.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_ChaseDetectPlayer::UBTService_ChaseDetectPlayer()
{
    NodeName = "Chase Player Detect";
    // 블랙보드 키 설정
    PlayerDetectedKey.AddBoolFilter(this, FName("PlayerDetected"));
    PlayerLocationKey.AddVectorFilter(this, FName("PlayerLocation"));
    // 근접 공격 관련 키는 제거합니다.
}

void UBTService_ChaseDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!BlackboardComp || !AIController) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn)
    {
        // 플레이어가 없으면 블랙보드 키 초기화
        BlackboardComp->SetValueAsBool(PlayerDetectedKey.SelectedKeyName, false);
        BlackboardComp->ClearValue(PlayerLocationKey.SelectedKeyName);
        return;
    }

    // AI의 위치와 플레이어의 위치 사이의 거리 계산
    float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());

    // 플레이어가 탐지되었는지 여부 확인 (예: 2000.0f 거리 내)
    // 이 값은 비헤이비어 트리의 서비스 노드에서 수정 가능하도록 변수화하는 것이 좋습니다.
    bool bPlayerDetected = Distance < 2000.0f;

    // 플레이어 탐지 여부 블랙보드에 저장
    BlackboardComp->SetValueAsBool(PlayerDetectedKey.SelectedKeyName, bPlayerDetected);

    if (bPlayerDetected)
    {
        // 플레이어 위치 블랙보드에 저장
        BlackboardComp->SetValueAsVector(PlayerLocationKey.SelectedKeyName, PlayerPawn->GetActorLocation());
    }
    else
    {
        // 플레이어가 탐지되지 않았으면 위치 정보 초기화
        BlackboardComp->ClearValue(PlayerLocationKey.SelectedKeyName);
    }
}
