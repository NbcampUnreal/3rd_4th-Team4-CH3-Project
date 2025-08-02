#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "PickUpWeaponMaster.generated.h"

class UPickUpComponent;
class ATestCharacter;

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
    FDataTableRowHandle WeaponData;

    // 현재 오버랩된 캐릭터 참조
    ATestCharacter* OverlappingCharacter;

    UFUNCTION()
    void HandlePickUp(AActor* PickUpActor);

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
    UStaticMeshComponent* StaticMesh;

public:
    void EquipWeapon();  // F 키 입력 시 호출할 함수
};
