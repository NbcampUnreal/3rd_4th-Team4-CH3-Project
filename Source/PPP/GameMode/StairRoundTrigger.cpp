
#include "StairRoundTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PPPGameMode.h"
#include "PPPGameState.h"
#include "GameFramework/Character.h"

AStairRoundTrigger::AStairRoundTrigger()
{
    PrimaryActorTick.bCanEverTick = false; // Tick 비활성화(성능 최적화)

    // BoxComponent 생성 및 루트 컴포넌트로 지정
    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    SetRootComponent(Box);

    // 콜리전 설정: Pawn 채널만 Overlap 허용
    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // 오버랩 이벤트 바인딩
    Box->OnComponentBeginOverlap.AddDynamic(this, &AStairRoundTrigger::OnBoxBeginOverlap);
}

void AStairRoundTrigger::BeginPlay()
{
    Super::BeginPlay();
}

// 플레이어가 박스에 진입했을 때 호출
void AStairRoundTrigger::OnBoxBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // 트리거가 비활성 상태이면 작동하지 않음
    if (!bEnabled) return;

    // 플레이어(캐릭터)만 허용
    if (!OtherActor || !OtherActor->IsA(ACharacter::StaticClass())) return;

    // GameMode / GameState 참조 가져오기
    APPPGameMode* GM = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this));
    APPPGameState* GS = Cast<APPPGameState>(UGameplayStatics::GetGameState(this));
    if (!GM || !GS) return;

    // 고민중.. 일단 아래로
    // 기존: bOnlyWhenRoundEnded && GS->GetCurrentState() != EGameState::RoundEnded 시 return
    // 현재: 조건과 관계없이 즉시 진행

    // =========================
    // [추가] 최종층 처리 로직
    // - bIsFinalFloor가 true일 때는 라운드 시작 대신 스테이지 클리어/이동을 수행
    // - bCallRoundClearedBeforeTravel=true: 이동 전에 GameMode의 OnRoundCleared() 호출
    // - bTravelOnFinalFloor=true && TargetLevelName 유효: 지정 맵으로 이동
    // - 그 외: 기본적으로 GameMode의 OnGameOver() 등으로 마무리(임시 처리)
    // =========================
    if (bIsFinalFloor)
    {
        // 필요 시 라운드 클리어 신호(라운드 증가/스코어 리셋/브로드캐스트 등)
        if (bCallRoundClearedBeforeTravel)
        {
            UE_LOG(LogTemp, Log, TEXT("[StairTrigger] FinalFloor: OnRoundCleared() 호출"));
            GM->OnRoundCleared();
        }

        // 최종층에서 다른 맵으로 이동
        if (bTravelOnFinalFloor && TargetLevelName != NAME_None)
        {
            UE_LOG(LogTemp, Log, TEXT("[StairTrigger] FinalFloor: OpenLevel -> %s"), *TargetLevelName.ToString());
            UGameplayStatics::OpenLevel(this, TargetLevelName);
        }
        else
        {
            // [임시 처리] 이동이 없다면 스테이지 종료 등 처리(추후 클리어 UI 연결 가능)
            UE_LOG(LogTemp, Log, TEXT("[StairTrigger] FinalFloor: 이동 없음 → OnGameOver() 호출(임시)"));
            GM->OnGameOver();
        }

        // 한 번만 작동하도록 설정되어 있으면 비활성화
        if (bConsumeOnce)
        {
            bEnabled = false;
            Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        return; // 최종층 처리는 여기서 종료
    }

    // 유효한 라운드 번호인지 확인
    if (RoundIndexToStart <= 0) return;

    // 현재 라운드를 지정된 라운드로 강제 설정
    GS->SetCurrentRound(RoundIndexToStart);

    // GameMode에 라운드 시작 요청
    UE_LOG(LogTemp, Log, TEXT("[StairTrigger] StartRound -> Round %d"), RoundIndexToStart);
    GM->StartRound();

    // 한 번만 작동하도록 설정된 경우: 비활성화 처리
    if (bConsumeOnce)
    {
        bEnabled = false;
        Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
