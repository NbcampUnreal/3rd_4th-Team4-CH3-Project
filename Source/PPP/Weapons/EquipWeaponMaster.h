#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponRow.h"
#include "PppCharacter.h"
#include "EquipWeaponMaster.generated.h"

UCLASS()
class PPP_API AEquipWeaponMaster : public AActor
{
	GENERATED_BODY()

public:
	AEquipWeaponMaster();

    // 캐릭터가 무기를 장착할 때 호출될 함수 (무기 정보와 캐릭터를 전달받음)
    UFUNCTION(BlueprintCallable)
    virtual void OnEquipped(class APppCharacter* NewOwner, const FWeaponRow& WeaponRow);

protected:

    // 기본적인 설정들. 씬루트, 스태틱 메쉬, 콜리전 생성
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Equip")
    USceneComponent* Scene;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Equip")
    USkeletalMeshComponent* SkeletalMesh;

    // 데이터 테이블 변수 생성, Data Table 과 Row Name을 설정할 수 있게 해줌
    UPROPERTY(EDITANYWHERE, BlueprintReadWrite, Category = "DataTable")
    FDataTableRowHandle WeaponData;

    // 무기의 기본 정보들 (데미지, 탄창 크기)
    float Damage;
    int32 MagazineSize;
};
