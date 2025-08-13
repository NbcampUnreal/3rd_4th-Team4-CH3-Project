
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

    // GameMode / GameState 참조
    APPPGameMode* GM = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this));
    APPPGameState* GS = Cast<APPPGameState>(UGameplayStatics::GetGameState(this));
    if (!GM || !GS) return;

    // -------- [최종 층 처리] --------
    if (bIsFinalFloor)
    {
        if (bCallRoundClearedBeforeTravel)
        {
            UE_LOG(LogTemp, Log, TEXT("[StairTrigger] FinalFloor: OnRoundCleared() 호출"));
            GM->OnRoundCleared();
        }

        if (bTravelOnFinalFloor && TargetLevelName != NAME_None)
        {
            UE_LOG(LogTemp, Log, TEXT("[StairTrigger] FinalFloor: OpenLevel -> %s"), *TargetLevelName.ToString());
            UGameplayStatics::OpenLevel(this, TargetLevelName);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[StairTrigger] FinalFloor: 이동 없음 → OnGameOver() 호출"));
            GM->OnGameOver();
        }

        if (bConsumeOnce)
        {
            bEnabled = false;
            Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        return; // 최종층 처리 끝
    }

    // -------- [일반 라운드 시작 처리] --------

    // 유효한 라운드 번호가 아니면 무시
    if (RoundIndexToStart <= 0) return;

    // 이미 시작된 라운드면 무시
    if (GM->IsRoundActive()) return;

    // 현재 라운드가 아닌 경우에만 세팅
    if (GM->GetCurrentRound() != RoundIndexToStart)
    {
        GS->SetCurrentRound(RoundIndexToStart);
    }

    // 라운드 시작
    UE_LOG(LogTemp, Log, TEXT("[StairTrigger] Round %d 시작"), RoundIndexToStart);
    GM->StartRound();

    // 소비형 트리거면 비활성화
    if (bConsumeOnce)
    {
        bEnabled = false;
        Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

