#include "PPPAIController.h"
#include "BehaviorTree/BlackboardComponent.h" // UBlackboardComponent 필요
#include "BehaviorTree/BehaviorTree.h"       // UBehaviorTree 필요
#include "BehaviorTree/BlackboardData.h"    // UBlackboardData 필요
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"        // UAISense_Sight::StaticClass()를 위해 필요
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Character.h"
#include "PPPAICharacter.h" // 필요한 경우 포함
#include "PPPFleeAICharacter.h" // 필요한 경우 포함
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h" // UGameplayStatics::GetPlayerPawn을 위해 필요

// 블랙보드 키 정의 및 초기화 (static const FName의 정의)
const FName APPPAIController::TargetActorKeyName = TEXT("TargetActor");
const FName APPPAIController::CanSeeTargetKeyName = TEXT("CanSeeTarget");
const FName APPPAIController::TargetLastKnownLocationKeyName = TEXT("TargetLastKnownLocation");
const FName APPPAIController::IsInvestigatingKeyName = TEXT("IsInvestigating");
const FName APPPAIController::PatrolLocationKeyName = TEXT("PatrolLocation");


// 생성자
APPPAIController::APPPAIController() // <-- 라인 204 근처일 가능성
{
    // ... 생성자 코드 내용 ...

    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    SetPerceptionComponent(*AIPerceptionComponent);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        SightConfig->SightRadius = ChaseSightRadius;
        SightConfig->LoseSightRadius = ChaseLoseSightRadius;
        SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
        AIPerceptionComponent->ConfigureSense(*SightConfig);
    }
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &APPPAIController::OnTargetPerceptionUpdated);

    BehaviorTreeAsset = nullptr;
    BlackboardDataAsset = nullptr;
} // <-- 생성자의 닫는 괄호 (이 괄호가 제대로 닫혀야 함)


// BeginPlay 함수 정의 (생성자 닫는 괄호 } 밖에 있어야 합니다!)
void APPPAIController::BeginPlay()
{
    Super::BeginPlay();
    // 이전에 비워두거나 삭제하라고 했던 내용 그대로 유지
}


// OnPossess 함수 정의 (생성자 닫는 괄호 } 밖에 있어야 합니다!)
void APPPAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // BehaviorTreeAsset과 BlackboardDataAsset이 블루프린트에서 설정되어 있는지 확인
    if (BehaviorTreeAsset && BlackboardDataAsset)
    {
        // UBlackboardComponent 포인터를 선언합니다.
        // UseBlackboard 함수가 이 포인터를 채워줄 것입니다.
        UBlackboardComponent* CurrentBlackboardComponent = nullptr;

        // UseBlackboard 함수를 호출하여 BlackboardComponent를 초기화하고 가져옵니다.
        // 첫 번째 인자는 UBlackboardData, 두 번째 인자는 UBlackboardComponent*에 대한 참조입니다.
        if (UseBlackboard(BlackboardDataAsset, CurrentBlackboardComponent)) // <-- 이 부분이 핵심!
        {
            // Blackboard가 성공적으로 초기화되고 가져와지면 비헤이비어 트리를 실행합니다.
            RunBehaviorTree(BehaviorTreeAsset);
            UE_LOG(LogTemp, Warning, TEXT("Behavior Tree Started!"));
        }
        else
        {
            // UseBlackboard 실패 시 로그
            UE_LOG(LogTemp, Error, TEXT("Blackboard를 초기화하지 못했습니다. BP_PPPAIController에서 BlackboardDataAsset을 확인하세요."));
        }
    }
    else
    {
        // 비헤이비어 트리 또는 블랙보드 데이터 애셋이 설정되지 않은 경우 오류 로그
        UE_LOG(LogTemp, Error, TEXT("OnPossess에서 BehaviorTreeAsset (%s) 또는 BlackboardDataAsset (%s)이(가) NULL입니다! BP_PPPAIController에서 BT_PPPAI와 BB_PPPAI를 설정하세요."),
            BehaviorTreeAsset ? *BehaviorTreeAsset->GetName() : TEXT("NULL"),
            BlackboardDataAsset ? *BlackboardDataAsset->GetName() : TEXT("NULL"));
    }
}


// OnTargetPerceptionUpdated - AI 퍼셉션 업데이트 콜백 (h 파일의 함수 이름과 일치하도록 수정)
void APPPAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn)
    {
        return;
    }

    // 자기 자신 인지 무시
    if (Actor == GetPawn())
    {
        return;
    }

    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return;
    }

    // 인지된 액터가 플레이어인지 확인
    if (Actor == PlayerPawn)
    {
        // 자각이 시야 센스에서 온 것인지 확인
        if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
        {
            // 플레이어가 성공적으로 인지(목격)된 경우
            if (Stimulus.WasSuccessfullySensed())
            {
                BlackboardComp->SetValueAsObject(TargetActorKeyName, Actor);
                BlackboardComp->SetValueAsBool(CanSeeTargetKeyName, true);
                BlackboardComp->SetValueAsVector(TargetLastKnownLocationKeyName, Actor->GetActorLocation());

                DrawDebugString(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 100),
                                FString::Printf(TEXT("Saw CHASE: %s"), *Actor->GetName()),
                                nullptr, FColor::Yellow, 5.f);
                UE_LOG(LogTemp, Warning, TEXT("AI : Player Sensed (CHASE) - CanSeeTarget: true"));

                BlackboardComp->SetValueAsBool(IsInvestigatingKeyName, false); // 플레이어가 보이면 조사 중단
            }
            // 플레이어 시야에서 벗어난 경우
            else
            {
                BlackboardComp->SetValueAsBool(CanSeeTargetKeyName, false);
                // TargetActor를 마지막으로 알려진 위치로 유지할지, 아니면 지울지 디자인에 따라 결정
                // BlackboardComp->SetValueAsObject(TargetActorKeyName, nullptr);

                DrawDebugString(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 100),
                                FString::Printf(TEXT("Missed CHASE: %s"), *Actor->GetName()),
                                nullptr, FColor::Red, 5.f);
                UE_LOG(LogTemp, Warning, TEXT("AI : Player Lost Sight (CHASE) - CanSeeTarget: false"));

                BlackboardComp->SetValueAsBool(IsInvestigatingKeyName, true); // 플레이어를 놓치면 조사 시작
            }
        }
    }
    // FleeAICharacter에 대한 특정 로직 (필요한 경우, 그렇지 않으면 이 else-if 블록 제거)
    else if (Cast<APppFleeAICharacter>(GetPawn())) // PPPFleeAICharacter.h가 포함되어 있는지 확인
    {
        if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
        {
            if (Stimulus.WasSuccessfullySensed())
            {
                UE_LOG(LogTemp, Warning, TEXT("Flee AI : Player Sensed"));
                BlackboardComp->SetValueAsBool(CanSeeTargetKeyName, true);
                BlackboardComp->SetValueAsObject(TargetActorKeyName, Actor);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Flee AI : Player Lost"));
                BlackboardComp->SetValueAsBool(CanSeeTargetKeyName, false);
                BlackboardComp->SetValueAsObject(TargetActorKeyName, nullptr);
            }
        }
    }
}
