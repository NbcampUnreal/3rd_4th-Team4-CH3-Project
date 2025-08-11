// StageTravelTrigger.cpp
#include "StageTravelTrigger.h"
#include "Kismet/GameplayStatics.h"
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

    // 원한다면 SeamlessTravel 옵션으로 전환 가능(프로젝트 세팅/GM 옵션 조정)
    UGameplayStatics::OpenLevel(this, TargetLevelName);
}
