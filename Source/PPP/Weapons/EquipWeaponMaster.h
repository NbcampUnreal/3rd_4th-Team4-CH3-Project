#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
};
