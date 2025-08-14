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
    if (bTriggered) return; // 중복 방지
    if (!bTravelEnabled) return;
    if (!OtherActor || !OtherActor->IsA(ACharacter::StaticClass())) return;
    if (TargetLevelName.IsNone()) return;

    // 라운드 클리어 처리 (단, StartRound는 호출 금지!)
    if (bCallRoundClearedOnTravel)
    {
        if (APPPGameMode* GM = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            GM->FlagRoundClearedWithoutStarting(); // <- 안전한 라운드 클리어 처리
        }
    }

    bTriggered = true;

    UGameplayStatics::OpenLevel(this, TargetLevelName);
}
