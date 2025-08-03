#pragma once

#include "CoreMinimal.h"
#include "WeaponRow.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    // enum 설정
    Pistol UMETA(DisplayName = "Pistol"),
    Assault_Rifle UMETA(DisplayName = "Assault_Rifle"),
    Shotgun UMETA(DisplayName = "Shotgun"),
    Rocket_Launcher UMETA(DisplayName = "Rocket_Launcher"),
    UnArmed UMETA(DisplayName = "UnArmed")
};


USTRUCT(BlueprintType)
struct FWeaponRow : public FTableRowBase
{
    GENERATED_BODY()

// 데이터 테이블의 행 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MagazineSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> PickUpWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> EquipWeapon;
};
