#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "WeaponRow.h"
#include "PickUpWeaponMaster.generated.h"

class UPickUpComponent;
class AEquipWeaponMaster;
class APppCharacter;

UCLASS()
class PPP_API APickUpWeaponMaster : public AActor
{
    GENERATED_BODY()

public:
    APickUpWeaponMaster();
    virtual void BeginPlay() override;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
    UPickUpComponent* PickUpComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
    FWeaponRow WeaponRow;

    APppCharacter* OverlappingCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
    FDataTableRowHandle WeaponData;

    UFUNCTION()
    void HandlePickUp(APppCharacter* PickUpCharacter);

    FWeaponRow GetWeaponDataRow() const { return WeaponRow; }

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
    UStaticMeshComponent* StaticMesh;
};
