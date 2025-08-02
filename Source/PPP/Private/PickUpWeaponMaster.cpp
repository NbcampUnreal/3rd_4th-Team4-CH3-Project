#include "PickUpWeaponMaster.h"
#include "PickUpComponent.h"
#include "TestCharacter.h"
#include "Engine/DataTable.h"

APickUpWeaponMaster::APickUpWeaponMaster()
{
    PrimaryActorTick.bCanEverTick = false;

    PickUpComp = CreateDefaultSubobject<UPickUpComponent>(TEXT("PickUpComp"));
    RootComponent = PickUpComp;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(RootComponent);

    OverlappingCharacter = nullptr;
}

void APickUpWeaponMaster::BeginPlay()
{
    Super::BeginPlay();

    if (PickUpComp)
    {
        // 델리게이트 바인딩 추가
        PickUpComp->WeaponPickUp.AddDynamic(this, &APickUpWeaponMaster::HandlePickUp);
    }
}

void APickUpWeaponMaster::HandlePickUp(AActor* PickUpActor)
{
    ATestCharacter* Character = Cast<ATestCharacter>(PickUpActor);
    if (Character)
    {
        OverlappingCharacter = Character;
        Character->OverlappingPickUpActor = this;
    }
}

void APickUpWeaponMaster::EquipWeapon()
{
    if (OverlappingCharacter)
    {
        OverlappingCharacter->EquipWeaponFromRow(WeaponData);
        Destroy();
    }
}
