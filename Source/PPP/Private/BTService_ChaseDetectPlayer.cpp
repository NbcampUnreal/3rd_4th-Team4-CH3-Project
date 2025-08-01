#include "BTService_ChaseDetectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "PppChaseAICharacter.h" // A_ChaseAICharacter에서 SightRadius를 가져옴
#include "PppChaseAIController.h"

UBTService_ChaseDetectPlayer::UBTService_ChaseDetectPlayer()
{
    bNotifyBecomeRelevant = true;
    NodeName = TEXT("Chase Player Detect");

    PlayerLocationKey.AddVectorFilter(
        this,
        GET_MEMBER_NAME_CHECKED(UBTService_ChaseDetectPlayer, PlayerLocationKey));
    PlayerDetectedKey.AddBoolFilter(
        this,
        GET_MEMBER_NAME_CHECKED(UBTService_ChaseDetectPlayer, PlayerDetectedKey));
}

void UBTService_ChaseDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    APppChaseAICharacter* ChaseCharacter = Cast<APppChaseAICharacter>(AIController ? AIController->GetPawn() : nullptr);
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!ChaseCharacter || !BlackboardComp) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn)
    {
        BlackboardComp->SetValueAsBool(PlayerDetectedKey.SelectedKeyName, false);
        return;
    }

    float DistanceToPlayer = FVector::Distance(ChaseCharacter->GetActorLocation(), PlayerPawn->GetActorLocation());
    bool bPlayerInSight = DistanceToPlayer <= ChaseCharacter->SightRadius;

    BlackboardComp->SetValueAsBool(PlayerDetectedKey.SelectedKeyName, bPlayerInSight);

    if (bPlayerInSight)
    {
        BlackboardComp->SetValueAsVector(PlayerLocationKey.SelectedKeyName, PlayerPawn->GetActorLocation());
    }
}

