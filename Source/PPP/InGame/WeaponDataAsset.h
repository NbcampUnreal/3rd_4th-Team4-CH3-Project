// by Yeoul
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Weapons/WeaponTypes.h"
#include "WeaponDataAsset.generated.h"

UCLASS(BlueprintType)
class PPP_API UWeaponDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
    FText WeaponName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
    int32 MaxMagSize = 30;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
    EFireMode FireMode = EFireMode::Single; // 라이플만 Auto로 바꿔서 사용

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
    UTexture2D* WeaponThumbnail = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
    UTexture2D* AmmoThumbnail = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class")
    TSubclassOf<class AEquipWeaponMaster> EquipWeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class")
    TSubclassOf<class APickUpWeaponMaster> PickupWeaponClass;
};
