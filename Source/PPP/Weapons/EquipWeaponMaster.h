#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "WeaponRow.h"
#include "EquipWeaponMaster.generated.h"

// 무기 발사 시 피격(라인 트레이스 결과) 정보를 브로드캐스트하는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFired, const FHitResult&, HitResult);

// 무기 드랍 시, 드랍된 무기 자신을 브로드캐스트하는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponDropped, AEquipWeaponMaster*, DroppedWeapon);

// 플레이어가 장착하는 무기(장착/발사/드랍 동작과 스탯 적용을 담당)
UCLASS()
class PPP_API AEquipWeaponMaster : public AActor
{
    GENERATED_BODY()

public:
    AEquipWeaponMaster();

    // 발사 이벤트: 블루프린트에서 바인딩 가능 (피격 결과 전달)
    UPROPERTY(BlueprintAssignable)
    FOnWeaponFired OnWeaponFired;

    // 드랍 이벤트: 블루프린트에서 바인딩 가능 (드랍된 무기 참조 전달)
    UPROPERTY(BlueprintAssignable)
    FOnWeaponDropped OnWeaponDropped;

    // 무기 발사: 화면 중앙 기준 라인트레이스, 피격 처리 및 이펙트/디버그 라인 출력
    UFUNCTION(BlueprintCallable)
    void Fire();

    // 무기 장착 처리: 소유자 설정, 무기사양(WeaponRow) 반영, 손 소켓에 부착 및 오프셋 적용
    UFUNCTION(BlueprintCallable)
    virtual void OnEquipped(class APppCharacter* NewOwner, const FWeaponRow& InWeaponRow);

    // 무기 드랍: 소유자 해제, 물리 시뮬레이션 활성화, 드랍 이벤트 브로드캐스트
    UFUNCTION(BlueprintCallable)
    void Drop();

    // 현재 무기 정보(행 데이터) 보관 및 접근자 제공
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponData")
    FWeaponRow WeaponDataRow;

    // 현재 장착 무기의 데이터 행 반환
    FWeaponRow GetWeaponDataRow() const { return WeaponDataRow; }

    // 발사 시 재생할 스켈레탈 애니메이션(총구 반동/화염 등 표현)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animation")
    UAnimSequence* FireAnim;

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Equip")
    USceneComponent* Scene;

    // 무기 외형 및 소켓(총구 등)을 제공하는 스켈레탈 메시
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Equip")
    USkeletalMeshComponent* SkeletalMesh;

    // 데이터테이블 핸들: 에디터에서 선택한 Row를 참조용으로 보관할 수 있음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
    FDataTableRowHandle WeaponData;

    // 무기 스탯: 장착 시 WeaponRow로부터 매핑되어 사용
    float Damage;
    int32 MagazineSize;
    float ReloadTime;
    FName WeaponName;
    float FireRange;
    int32 WeaponIndex;
};
