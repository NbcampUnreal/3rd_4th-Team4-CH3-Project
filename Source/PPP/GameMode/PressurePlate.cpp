#include "PressurePlate.h"
#include "Components/BoxComponent.h"
#include "DoorActor.h"

APressurePlate::APressurePlate()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);
    TriggerBox->SetBoxExtent(FVector(50, 50, 10));
    TriggerBox->SetCollisionProfileName("Trigger");

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnOverlapBegin);
}

void APressurePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
                                    AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex,
                                    bool bFromSweep,
                                    const FHitResult& SweepResult)
{
    if (LinkedDoor)
    {
        LinkedDoor->OpenDoor();
    }
}
