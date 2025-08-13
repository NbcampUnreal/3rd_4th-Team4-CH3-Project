#include "BTService_FleeDetectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "PppFleeAICharacter.h"
#include "PppFleeAIController.h"
#include "Engine/Engine.h"

UBTService_FleeDetectPlayer::UBTService_FleeDetectPlayer()
{
    bNotifyBecomeRelevant = true;
    NodeName = TEXT("Flee Player Detect");

    PlayerLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FleeDetectPlayer, PlayerLocationKey));
    PlayerDetectedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FleeDetectPlayer, PlayerDetectedKey));
    // TriggerFleeDistance와 StopFleeDistance
    // TriggerFleeDistance = 800.0f;
    // StopFleeDistance = 1500.0f;
}

void UBTService_FleeDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    APppFleeAICharacter* FleeCharacter = Cast<APppFleeAICharacter>(AIController ? AIController->GetPawn() : nullptr);
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (!FleeCharacter || !BlackboardComp)
    {
        if (BlackboardComp)
        {
            BlackboardComp->ClearValue(PlayerDetectedKey.SelectedKeyName);
        }
        return;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn)
    {
        BlackboardComp->ClearValue(PlayerDetectedKey.SelectedKeyName);
        return;
    }

    float DistanceToPlayer = FVector::Distance(FleeCharacter->GetActorLocation(), PlayerPawn->GetActorLocation());

    float ActualDetectionRadius = FleeCharacter->SightRadius;
    if (ActualDetectionRadius <= 0.0f) {
        ActualDetectionRadius = 2000.0f;
    }

    bool bPlayerDetected = DistanceToPlayer <= ActualDetectionRadius;

    if (bPlayerDetected)
    {
        BlackboardComp->SetValueAsBool(PlayerDetectedKey.SelectedKeyName, true);
        BlackboardComp->SetValueAsVector(PlayerLocationKey.SelectedKeyName, PlayerPawn->GetActorLocation());
        //UE_LOG(LogTemp, Warning, TEXT("FleeAI: Player DETECTED within %.2f. Distance: %.2f"), ActualDetectionRadius, DistanceToPlayer);
    }
    else
    {
        BlackboardComp->ClearValue(PlayerDetectedKey.SelectedKeyName);
        BlackboardComp->ClearValue(PlayerLocationKey.SelectedKeyName);
        //UE_LOG(LogTemp, Warning, TEXT("FleeAI: Player NOT DETECTED. Distance: %.2f"), DistanceToPlayer);
    }
}
