#include "PPPAIController.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "PPPAICharacter.h"
//#include "PPPFleeAICharacter.h"

APPPAIController::APPPAIController()
{
	// AI 컴포넌트 생성 및 설정
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	// 기본 컴포넌트로 설정
	SetPerceptionComponent(*AIPerception);

	// 시야 감지 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f; //시야 반경
	SightConfig->LoseSightRadius = 2000.0f; //시야를 잃는 반경
	SightConfig->PeripheralVisionAngleDegrees = 90.0f; // 주변 시야 각도
	SightConfig->SetMaxAge(5.0f); // 감지 정보 유지 시간
	//AI 감지대상 설정(아군, 적, 중립 모두 감지)
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	//AIPerception 컴포넌트에 시야 설정 적용
	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}
void APPPAIController::BeginPlay()
{
	Super::BeginPlay();

	// AIPerception 컴포넌트가 유효하면, 감지 업데이트 이벤트에 바인딩
	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(
			this,
			&APPPAIController::OnPerceptionUpdated // 플레이어를 감지하거나 놓쳤을 때 호출
		);
	}

	// 타이머 구현 (초기 1초, 반복 간격 3초)
	GetWorldTimerManager().SetTimer(
		RandomMoveTimer,
		this,
		&APPPAIController::MoveToRandomLocation,
		3.0f, // 3초마다
		true, // 반복
		1.0f //1초 후 실행
	);
}

//AIController가 캐릭터를 소유할 때 호출할 함수
void APPPAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PPP] AI Controller is controlling %s."), *InPawn->GetName());
	}
}

// 공통 랜덤 이동 로직
void APPPAIController::MoveToRandomLocation()
{
	APawn* MyPawn = GetPawn(); // 현재 컨트롤러가 소유한 Pawn 가져오기
	if (!MyPawn)
	{
		return;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		return;
	}

	FNavLocation RandomLocation; // 랜덤 위치를 저장할 변수

	// 현재 위치에서 반경 내의 도달 가능한 랜덤 위치 찾기
	bool bFoundLocation = NavSystem->GetRandomReachablePointInRadius(
		MyPawn->GetActorLocation(),
		MoveRadius,
		RandomLocation
		);

	if (bFoundLocation)
	{
		MoveToLocation(RandomLocation.Location); // 찾은 위치로 이동
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Sparta] Could not find a reachable location."));
	}
}

// 플레이어가 감지 시 호출되며 AI 타입에 따라 행동 결정
void APPPAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	// 발견한 액터가 플레이어인지 감지 아니면 무시
	if (Actor != PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PPP] 플레이어 외의 액터를 감지함: %s"), *Actor->GetName());
		return;
	}
	//
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PPP] 플레이어 감지됨! %s 추격 시작."), *Actor->GetName());
		DrawDebugString(
			GetWorld(),
			Actor->GetActorLocation() + FVector(0, 0, 100),
			FString::Printf(TEXT("Saw: %s"), *Actor->GetName()),
			nullptr,
			FColor::Green,
			2.0f,
			true
		);

		StartChasing(Actor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PPP] 플레이어 놓침! %s 추격 중지."), *Actor->GetName());
		DrawDebugString(
			GetWorld(),
			Actor->GetActorLocation() + FVector(0, 0, 100),
			FString::Printf(TEXT("Missed: %s"), *Actor->GetName()),
			nullptr,
			FColor::Red,
			2.0f,
			true
		);

		StopChasing();
	}
}
// 추격 관련 AI 함수들
void APPPAIController::StartChasing(AActor* Target)
{
	if (bIsChasing && CurrentTarget == Target) return;

	CurrentTarget = Target;
	bIsChasing = true;
	bIsFleeing = false; // 도망 중이라면 중지

	GetWorldTimerManager().ClearTimer(RandomMoveTimer); // 랜덤 이동 타이머 중지

	if (APPPAICharacter* AIChar = Cast<APPPAICharacter>(GetPawn()))
	{
		AIChar->SetMovementSpeed(AIChar->RunSpeed); // 이동 속도 RunSpeed로 설정
	}

	UpdateChase(); // 즉시 추격 방향으로 한 번 이동 시도

	GetWorldTimerManager().SetTimer(
		ChaseTimer,
		this,
		&APPPAIController::UpdateChase,
		0.25f,
		true
		);
	UE_LOG(LogTemp, Warning, TEXT("[PPP] ChaseTimer 설정됨."));

}

void APPPAIController::StopChasing()
{

	if (!bIsChasing)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PPP] 추격 중이 아니므로 StopChasing은 아무것도 하지 않음."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[PPP] StopChasing 진입."));
	CurrentTarget = nullptr;
	bIsChasing = false;

	GetWorldTimerManager().ClearTimer(ChaseTimer);
	UE_LOG(LogTemp, Warning, TEXT("[PPP] ChaseTimer 클리어됨."));

	StopMovement();
	UE_LOG(LogTemp, Warning, TEXT("[PPP] 이동 중지됨."));

	if (APPPAICharacter* AIChar = Cast<APPPAICharacter>(GetPawn()))
	{
		AIChar->SetMovementSpeed(AIChar->WalkSpeed);
		UE_LOG(LogTemp, Warning, TEXT("[PPP] AI 캐릭터 속도 WalkSpeed로 설정됨: %.1f"), AIChar->WalkSpeed);

	}

	GetWorldTimerManager().SetTimer(
		RandomMoveTimer,
		this,
		&APPPAIController::MoveToRandomLocation,
		3.0f,
		true,
		2.0f
	);
	UE_LOG(LogTemp, Warning, TEXT("[PPP] RandomMoveTimer가 2초 초기 지연으로 설정됨."));

}

void APPPAIController::UpdateChase()
{
	if (CurrentTarget && bIsChasing)
	{
		MoveToActor(CurrentTarget, 100.0f);
	}
}
