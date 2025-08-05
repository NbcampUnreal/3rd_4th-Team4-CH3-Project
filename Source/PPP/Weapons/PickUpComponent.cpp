#include "PickUpComponent.h"
#include "../Characters/PppCharacter.h"
#include "../Weapons/PickUpWeaponMaster.h"

UPickUpComponent::UPickUpComponent()
{
    SetGenerateOverlapEvents(true);
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionObjectType(ECC_WorldDynamic);
    SetCollisionResponseToAllChannels(ECR_Ignore);
    SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UPickUpComponent::BeginPlay()
{
    Super::BeginPlay();

    OnComponentBeginOverlap.AddDynamic(this, &UPickUpComponent::OnBeginOverlap);
    OnComponentEndOverlap.AddDynamic(this, &UPickUpComponent::OnEndOverlap);
}

void UPickUpComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex,
                                      bool bFromSweep,
                                      const FHitResult& SweepResult)
{
    APppCharacter* PppCharacter = Cast<APppCharacter>(OtherActor);
    APickUpWeaponMaster* WeaponActor = Cast<APickUpWeaponMaster>(GetOwner());
    if (PppCharacter && WeaponActor)
    {
        WeaponActor->OverlappingCharacter = PppCharacter;
        PppCharacter->OverlappingPickUpActor = WeaponActor;
    }
}

void UPickUpComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
                                    AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex)
{
    APppCharacter* PppCharacter = Cast<APppCharacter>(OtherActor);
    APickUpWeaponMaster* WeaponActor = Cast<APickUpWeaponMaster>(GetOwner());
    if (PppCharacter && WeaponActor)
    {
        if (PppCharacter->OverlappingPickUpActor == WeaponActor)
            PppCharacter->OverlappingPickUpActor = nullptr;
    }
}

void UPickUpComponent::TryPickUp(APppCharacter* PickUpCharacter)
{
    WeaponPickUp.Broadcast(PickUpCharacter);
}
