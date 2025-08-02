#include "EquipWeaponMaster.h"
#include "Components/SkeletalMeshComponent.h"
#include "TestCharacter.h"

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

void AEquipWeaponMaster::OnEquipped(ATestCharacter* NewOwner, const FWeaponRow& WeaponRow)
{
    // 무기의 데미지 및 탄창 크기를 데이터 테이블로부터 설정
    Damage = WeaponRow.Damage;
    MagazineSize = WeaponRow.MagazineSize;

    // 무기를 캐릭터의 SkeletalMesh의 WeaponSocket에 붙임
    AttachToComponent(NewOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));

    // 무기의 소유자 설정
    SetOwner(NewOwner);

    UE_LOG(LogTemp, Warning, TEXT("무기 장착 완료: %s"), *WeaponRow.WeaponName.ToString());
}
