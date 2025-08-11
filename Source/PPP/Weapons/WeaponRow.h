#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"   // by Yeoul: 데이터 테이블 사용
#include "WeaponTypes.h"    // by Yeoul: EWeaponType Enum 사용
#include "WeaponRow.generated.h"

class UWeaponDataAsset; // by Yeoul: 무기 데이터 에셋 사용

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
    FName WeaponName;  // 무기 이름

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WeaponIndex = 0;  // 무기 고유 번호

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;  // 무기 종류

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;  // 무기 데미지

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MagazineSize;  // 무기 1탄찬당 총알 개수

    UPROPERTY(EDITANYWHERE, BlueprintReadWrite)
    float ReloadTime;  // 장전 시간

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FireRange;  // 유효 사거리

    // by Yeoul
    // PDA SoftRef: 런타임은 PDA만 진실원으로 사용
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
    TSoftObjectPtr<UWeaponDataAsset> WeaponData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class APickUpWeaponMaster> PickUpWeapon;  // 줍는 무기 종류, Static Mesh 타입

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class AEquipWeaponMaster> EquipWeapon;  // 장착 중인 무기, Skeletal Mesh 타입

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector WeaponOffset = FVector::ZeroVector;  // 무기 잡는 손 위치 값 저장 타입

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator WeaponRotation = FRotator::ZeroRotator;  // 무기 잡는 손 위치 값 저장 타입

    // by Yeoul
    // 예비 탄약 수, 예) 10 / 1000 이라 가정할 때 1000에 해당됨.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ReserveAmmo = 0;;
};
