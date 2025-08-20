#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "WeaponRow.h"
#include "PickUpWeaponMaster.generated.h"

class UPickUpComponent;
class AEquipWeaponMaster;
class APppCharacter;

// 무기 픽업 액터 클래스: 게임 내에서 무기 아이템으로 동작하며, 플레이어가 획득 가능
UCLASS()
class PPP_API APickUpWeaponMaster : public AActor
{
    GENERATED_BODY()

public:
    APickUpWeaponMaster();
    virtual void BeginPlay() override;

    // 픽업 컴포넌트: 무기와의 상호작용(겹침, 이벤트 트리거) 담당
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
    UPickUpComponent* PickUpComp;

    // 현재 무기 정보(구조체): DataTable에서 로드된 결과가 여기에 저장됨
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
    FWeaponRow WeaponRow;

    // 현재 픽업 영역에 겹쳐 있는 캐릭터를 저장
    APppCharacter* OverlappingCharacter;

    // 무기 데이터(DataTable RowHandle): DataTable에서 해당 Row를 찾아 무기 정보를 로딩
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
    FDataTableRowHandle WeaponData;

    // 실제 픽업 처리 함수: 캐릭터가 무기와 상호작용 시 호출됨
    UFUNCTION()
    void HandlePickUp(APppCharacter* PickUpCharacter);

    // 현재 무기 정보 구조체 반환(읽기 전용)
    FWeaponRow GetWeaponDataRow() const { return WeaponRow; }

protected:
    // 월드 내에 표시될 무기 메시(StaticMesh): PickUpActor의 외형을 담당
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
    UStaticMeshComponent* StaticMesh;
};
