#include "PickUpWeaponMaster.h"
#include "PickUpComponent.h"
#include "../Characters/PppCharacter.h"
#include "Engine/DataTable.h"
#include "../Weapons/EquipWeaponMaster.h"

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

    if (WeaponData.DataTable && !WeaponData.RowName.IsNone())
    {
        const FWeaponRow* LoadedRow = WeaponData.DataTable->FindRow<FWeaponRow>(
            WeaponData.RowName, TEXT("Get WeaponRow from DataTable"));
        if (LoadedRow)
        {
            WeaponRow = *LoadedRow; // 반드시 WeaponRow 구조체 복사!
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("무기 데이터테이블에서 Row 찾기 실패!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponData DataTable/RowName이 None임!"));
    }

    if (PickUpComp)
    {
        PickUpComp->WeaponPickUp.AddDynamic(this, &APickUpWeaponMaster::HandlePickUp);
    }
}

void APickUpWeaponMaster::HandlePickUp(APppCharacter* Character)
{
    if (!Character) return;

    if (Character->EquippedWeapon)
    {
        FWeaponRow PrevWeaponRow = Character->EquippedWeapon->GetWeaponDataRow();
        FVector DropLocation = Character->EquippedWeapon->GetActorLocation();
        FRotator DropRotation = Character->EquippedWeapon->GetActorRotation();

        Character->DropWeaponToWorld(PrevWeaponRow, DropLocation, DropRotation);

        Character->EquippedWeapon->Destroy();
        Character->EquippedWeapon = nullptr;
    }

    if (WeaponRow.EquipWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("스폰 시도 - Skeletal Weapon: %s"), *WeaponRow.EquipWeapon->GetName());
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Character;

        AEquipWeaponMaster* NewWeapon = Character->GetWorld()->SpawnActor<AEquipWeaponMaster>(
            WeaponRow.EquipWeapon,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParams
        );
        if (NewWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("NewWeapon 스폰 성공! OnEquipped 호출"));
            NewWeapon->OnEquipped(Character, WeaponRow);
            Character->EquippedWeapon = NewWeapon;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("NewWeapon SK 스폰 실패"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponRow.EquipWeapon이 None/Invalid"));
    }

    // 3. PickUp무기(PickUpWeaponMaster) Destroy
    Destroy();

    // 4. 캐릭터의 OverlappingPickUpActor 초기화
    Character->OverlappingPickUpActor = nullptr;
}
