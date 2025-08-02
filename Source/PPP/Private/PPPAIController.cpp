/*
#include "PPPAIController.h"
#include "NavigationSystem.h" // UNavigationSystemV1 사용
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "PPPAICharacter.h"       // 추격 AI 캐릭터 헤더
#include "PPPFleeAICharacter.h"   // 도망 AI 캐릭터 헤더
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

// AI 컨트롤러 생성자
APPPAIController::APPPAIController()
{
    // 블랙보드 컴포넌트 생성
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));

    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerception);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    // 임시 기본값 설정 (OnPossess에서 실제 AI 타입에 따라 재설정됨)
    SightConfig->SightRadius = 1500.0f;
    SightConfig->LoseSightRadius = 2000.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->SetMaxAge(5.0f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));
}

void APPPAIController::BeginPlay()
{
    Super::BeginPlay();

    if (BlackboardComp)
    {
        // 블랙보드 초기값 설정 (비헤이비어 트리가 시작되기 전에 설정)
        BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
        BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
        // 필요한 다른 블랙보드 키 초기화
    }

    if (AIPerception)
    {
       AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &APPPAIController::OnPerceptionUpdated);
    }
}

void APPPAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (InPawn)
    {
       UE_LOG(LogTemp, Warning, TEXT("[PPP] AI Controller is controlling %s."), *InPawn->GetName());

       // 빙의된 캐릭터 타입에 따라 시야 설정 적용
       if (Cast<APPPAICharacter>(InPawn)) // 추격 AI 캐릭터
       {
           SightConfig->SightRadius = ChaseSightRadius;
           SightConfig->LoseSightRadius = ChaseLoseSightRadius;
           SightConfig->PeripheralVisionAngleDegrees = ChasePeripheralVisionAngleDegrees;
           SightConfig->SetMaxAge(ChaseMaxAge);
           UE_LOG(LogTemp, Warning, TEXT("[PPP_AIConfig] Chase AI Sight Config applied. LoseSightRadius: %.1f"), ChaseLoseSightRadius);
       }
       else if (Cast<APPPFleeAICharacter>(InPawn)) // 도망 AI 캐릭터
       {
           SightConfig->SightRadius = FleeSightRadius;
           SightConfig->LoseSightRadius = FleeLoseSightRadius;
           SightConfig->PeripheralVisionAngleDegrees = FleePeripheralVisionAngleDegrees;
           SightConfig->SetMaxAge(FleeMaxAge);
           UE_LOG(LogTemp, Warning, TEXT("[PPP_AIConfig] Flee AI Sight Config applied. LoseSightRadius: %.1f"), FleeLoseSightRadius);
       }

       // 변경된 설정을 AIPerception 컴포넌트에 다시 적용
       AIPerception->ConfigureSense(*SightConfig);

       // *** 비헤이비어 트리 및 블랙보드 초기화 및 실행 ***
       if (BehaviorTreeAsset) // BehaviorTreeAsset이 블루프린트에서 할당되었는지 확인
       {
           // 블랙보드 초기화: BehaviorTreeAsset에 연결된 BlackboardData 에셋으로 초기화
           if (BlackboardComp && BehaviorTreeAsset->BlackboardAsset)
           {
               if (!BlackboardComp->InitializeBlackboard(*BehaviorTreeAsset->BlackboardAsset))
               {
                   UE_LOG(LogTemp, Error, TEXT("[PPP_BT] Failed to initialize Blackboard with asset %s for %s."), *BehaviorTreeAsset->BlackboardAsset->GetName(), *InPawn->GetName());
                   return;
               }
           }
           else
           {
               UE_LOG(LogTemp, Warning, TEXT("[PPP_BT] BlackboardComp or BehaviorTreeAsset->BlackboardAsset is null for %s."), *InPawn->GetName());
           }

           // BehaviorTree 실행 (블랙보드 초기화 후에 실행)
           if (!RunBehaviorTree(BehaviorTreeAsset))
           {
               UE_LOG(LogTemp, Error, TEXT("[PPP_BT] Failed to run Behavior Tree %s for %s."), *BehaviorTreeAsset->GetName(), *InPawn->GetName());
           }
           else
           {
               UE_LOG(LogTemp, Warning, TEXT("[PPP_BT] Behavior Tree %s started for %s."), *BehaviorTreeAsset->GetName(), *InPawn->GetName());
           }
       }
       else
       {
           UE_LOG(LogTemp, Warning, TEXT("[PPP_BT] No Behavior Tree Asset assigned for %s. AI will not run Behavior Tree based logic."), *InPawn->GetName());
       }
    }
}

// 핵심: 플레이어 감지 시 호출되며, AI 캐릭터 타입에 상관없이 블랙보드만 업데이트합니다.
void APPPAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Actor != PlayerPawn)
    {
        return; // 감지된 액터가 플레이어가 아니면 무시
    }

    if (BlackboardComp)
    {
        if (Stimulus.WasSuccessfullySensed()) // 플레이어를 감지했다면
        {
            // 블랙보드 업데이트: 타겟 정보를 블랙보드에 씁니다.
            BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
            BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), true);
            BlackboardComp->SetValueAsVector(TEXT("TargetLastKnownLocation"), Actor->GetActorLocation());

            // 디버그 드로잉 (AI 타입별로 색상 구분)
            if (Cast<APPPAICharacter>(GetPawn())) // 추격 AI
            {
                // IsInvestigating은 플레이어를 다시 감지했으니 false로 설정
                BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
                DrawDebugString(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 100), FString::Printf(TEXT("Saw CHASE: %s"), *Actor->GetName()), nullptr, FColor::Green, 2.0f, true);
            }
            else if (Cast<APPPFleeAICharacter>(GetPawn())) // 도망 AI
            {
                // 도망 AI는 'IsInvestigating'을 사용하지 않을 수 있음
                // 필요한 경우 도망 AI 전용 블랙보드 키 (예: IsFleeing)를 설정
                DrawDebugString(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 100), FString::Printf(TEXT("Saw FLEE: %s"), *Actor->GetName()), nullptr, FColor::Blue, 2.0f, true);
            }
        }
        else // 플레이어를 놓쳤다면
        {
            // 블랙보드 업데이트: 타겟 정보를 해제하고 놓쳤음을 알림
            BlackboardComp->SetValueAsObject(TEXT("TargetActor"), nullptr);
            BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);

            // 디버그 드로잉
            if (Cast<APPPAICharacter>(GetPawn())) // 추격 AI
            {
                // 마지막 위치를 조사하도록 설정
                BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), true);
                DrawDebugString(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 100), FString::Printf(TEXT("Missed CHASE: %s"), *Actor->GetName()), nullptr, FColor::Red, 2.0f, true);
            }
            else if (Cast<APPPFleeAICharacter>(GetPawn())) // 도망 AI
            {
                // 도망 AI는 'IsInvestigating'을 사용하지 않을 수 있음
                DrawDebugString(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 100), FString::Printf(TEXT("Missed FLEE: %s"), *Actor->GetName()), nullptr, FColor::Red, 2.0f, true);
            }
        }
    }
}
*/




// PPPAIController.cpp

// PPPAIController.cpp
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
