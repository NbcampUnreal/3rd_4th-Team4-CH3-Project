#include "PppFleeAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PppFleeAICharacter.h"


const FName APppFleeAIController::PlayerLocationKey(TEXT("PlayerLocation"));
const FName APppFleeAIController::PlayerDetectedKey(TEXT("PlayerDetected"));
const FName APppFleeAIController::FleeLocationKey(TEXT("FleeLocation"));

APppFleeAIController::APppFleeAIController()
{
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void APppFleeAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    APppFleeAICharacter* FleeCharacter = Cast<APppFleeAICharacter>(InPawn);
    if (FleeCharacter && FleeCharacter->BehaviorTreeAsset)
    {
        BlackboardComponent->InitializeBlackboard(*FleeCharacter->BehaviorTreeAsset->BlackboardAsset);
        BehaviorTreeComponent->StartTree(*FleeCharacter->BehaviorTreeAsset);
    }
}

void APppFleeAIController::OnUnPossess()
{
    Super::OnUnPossess();
    BehaviorTreeComponent->StopTree();
}
