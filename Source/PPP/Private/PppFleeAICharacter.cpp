#include "PppFleeAICharacter.h"
#include "PppFleeAIController.h"
#include "Perception/AIPerceptionComponent.h" // AI Perception Component 헤더
#include "Perception/AISenseConfig_Sight.h"   // 시야 센서 설정 헤더
#include "BehaviorTree/BehaviorTree.h"       // BehaviorTreeAsset 사용을 위해 포함
#include "Kismet/GameplayStatics.h"          // UGameplayStatics 사용을 위해 포함
#include "GenericTeamAgentInterface.h"       // 팀 설정에 필요 (이전에 PppFleeAICharacter에 추가되지 않았다면 추가)
#include "BehaviorTree/BlackboardComponent.h"

APppFleeAICharacter::APppFleeAICharacter()
{
    //생성자
    AIControllerClass = APppFleeAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    MinFleeDistance = 6000.0f;   // 플레이어로부터 최소 60미터 이상 도망
    MaxFleeDistance = 10000.0f;   // 최대 도망 거리 (100미터)

    // 1. PerceptionComponent 생성 및 설정
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

    // 2. 시야 센서 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    PerceptionComponent->SetDominantSense(SightConfig->GetClass()); // 주 감각으로 시야 설정
    PerceptionComponent->ConfigureSense(*SightConfig);

    // 3. 시야 센서 세부 설정
    SightConfig->SightRadius = 2500.0f;     // 플레이어를 인지할 수 있는 최대 거리
    SightConfig->LoseSightRadius = 3000.0f; // 시야를 잃는 거리
    SightConfig->PeripheralVisionAngleDegrees = 360.0f; // ★ 중요: 시야각 설정 (0~360). 360으로 하면 모든 방향 인지.
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

    // 4. 감지 이벤트 콜백 함수 바인딩
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &APppFleeAICharacter::OnTargetPerceptionUpdated);
}

void APppFleeAICharacter::BeginPlay()
{
    Super::BeginPlay();

}

void APppFleeAICharacter::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // 감지된 액터가 플레이어인지 확인
    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
    {
        if (Actor == PlayerPawn)
        {
            APppFleeAIController* AIController = Cast<APppFleeAIController>(GetController());
            if (AIController && AIController->GetBlackboardComponent())
            {
                if (Stimulus.WasSuccessfullySensed())
                {
                    AIController->GetBlackboardComponent()->SetValueAsBool(AIController->PlayerDetectedKey, true);
                    AIController->GetBlackboardComponent()->SetValueAsVector(AIController->PlayerLocationKey, PlayerPawn->GetActorLocation());
                    UE_LOG(LogTemp, Warning, TEXT("Perception: Player DETECTED (Location: %s)!"), *PlayerPawn->GetActorLocation().ToString());
                }
                else
                {
                    AIController->GetBlackboardComponent()->ClearValue(AIController->PlayerDetectedKey);
                    AIController->GetBlackboardComponent()->ClearValue(AIController->PlayerLocationKey);
                    UE_LOG(LogTemp, Warning, TEXT("Perception: Player LOST!"));
                }
            }
        }
    }
}
