// StageTravelTrigger.cpp
#include "StageTravelTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "PPPGameMode.h" // APPPGameMode
#include "GameFramework/Character.h"

AStageTravelTrigger::AStageTravelTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    SetRootComponent(Box);
    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AStageTravelTrigger::BeginPlay()
{
    Super::BeginPlay();
    Box->OnComponentBeginOverlap.AddDynamic(this, &AStageTravelTrigger::OnBoxBeginOverlap);
}

void AStageTravelTrigger::OnBoxBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)

{
    if (!bTravelEnabled) return;
    if (!OtherActor || !OtherActor->IsA(ACharacter::StaticClass())) return;

    if (TargetLevelName.IsNone())
        return;
    // 라운드 클리어 먼저 처리
    if (bCallRoundClearedOnTravel)
    {
        if (APPPGameMode* GM = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            GM->OnRoundCleared(); // 라운드 증가/스코어 리셋/신호 브로드캐스트
        }
    }

    UGameplayStatics::OpenLevel(this, TargetLevelName);
}
