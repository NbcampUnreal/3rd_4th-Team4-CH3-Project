#include "EquipWeaponMaster.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Characters/PppCharacter.h"

AEquipWeaponMaster::AEquipWeaponMaster()
{
	PrimaryActorTick.bCanEverTick = false;

    // 씬 루트 컴포넌트 생성
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    // 씬 루트 컴포넌트 부착?
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(Scene);

}

void AEquipWeaponMaster::OnEquipped(APppCharacter* NewOwner, const FWeaponRow& WeaponRow)
{
    Damage = WeaponRow.Damage;
    MagazineSize = WeaponRow.MagazineSize;

    AttachToComponent(NewOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));
    SetOwner(NewOwner);

    UE_LOG(LogTemp, Warning, TEXT("무기 장착 완료: %s"), *WeaponRow.WeaponName.ToString());
}

