#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "WeaponRow.h"
#include "EquipWeaponMaster.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFired, const FHitResult&, HitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponDropped, AEquipWeaponMaster*, DroppedWeapon);

UCLASS()
class PPP_API AEquipWeaponMaster : public AActor
{
    GENERATED_BODY()

public:
    AEquipWeaponMaster();

    UPROPERTY(BlueprintAssignable)
    FOnWeaponFired OnWeaponFired;
    UPROPERTY(BlueprintAssignable)
    FOnWeaponDropped OnWeaponDropped;

    UFUNCTION(BlueprintCallable)
    void Fire();

    UFUNCTION(BlueprintCallable)
    virtual void OnEquipped(class APppCharacter* NewOwner, const FWeaponRow& InWeaponRow);

    UFUNCTION(BlueprintCallable)
    void Drop();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponData")
    FWeaponRow WeaponDataRow;

    FWeaponRow GetWeaponDataRow() const { return WeaponDataRow; }

    // 총기 발사 애니메이션 시퀀스 선언
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animation")
    UAnimSequence* FireAnim;

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Equip")
    USceneComponent* Scene;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Equip")
    USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
    FDataTableRowHandle WeaponData;

    float Damage;
    int32 MagazineSize;
    float ReloadTime;
    FName WeaponName;
    float FireRange;
    int32 WeaponIndex;
};
