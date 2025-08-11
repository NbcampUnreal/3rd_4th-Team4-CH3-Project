#include "StairRoundTrigger.h"
#include "PPPGameMode.h"
#include "PPPGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AStairRoundTrigger::AStairRoundTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    SetRootComponent(Box);
    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AStairRoundTrigger::BeginPlay()
{
    Super::BeginPlay();
    Box->OnComponentBeginOverlap.AddDynamic(this, &AStairRoundTrigger::OnBoxBeginOverlap);
}

void AStairRoundTrigger::OnBoxBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!bEnabled) return;
    if (!OtherActor || !OtherActor->IsA(ACharacter::StaticClass())) return;

    APPPGameState* GS = Cast<APPPGameState>(UGameplayStatics::GetGameState(this));
    APPPGameMode* GM = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GM || !GS) return;

    if (bAllowWhenRoundEndedOnly && GS->GetCurrentState() != EGameState::RoundEnded)
    {
        return;
    }

    // 지정 라운드로 시작(층 연동용)
    GM->StartRound(); // 최소 변경: 현재 구조는 StartRound가 라운드 카운트를 건드리지 않으므로 OK
}
