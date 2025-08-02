#include "PppChaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PppChaseAICharacter.h"

// 블랙보드 키 이름 정의
const FName APppChaseAIController::PlayerLocationKey = (TEXT("PlayerLocation"));
const FName APppChaseAIController::PlayerDetectedKey = (TEXT("PlayerDetected"));

APppChaseAIController::APppChaseAIController()
{
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void APppChaseAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    APppChaseAICharacter* ChaseCharacter = Cast<APppChaseAICharacter>(InPawn);
    if (ChaseCharacter && ChaseCharacter->BehaviorTreeAsset)
    {
        // 비헤이비어 트리에 연결된 블랙보드로 블랙보드 컴포넌트 초기화
        BlackboardComponent->InitializeBlackboard(*ChaseCharacter->BehaviorTreeAsset->BlackboardAsset);

        // 비헤이비어 트리 실행 시작
        BehaviorTreeComponent->StartTree(*ChaseCharacter->BehaviorTreeAsset);
    }
}

void APppChaseAIController::OnUnPossess()
{
    Super::OnUnPossess();
    BehaviorTreeComponent->StopTree();
}


