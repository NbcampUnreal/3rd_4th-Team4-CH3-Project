#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "TestCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AEquipWeaponMaster;

UCLASS()
class PPP_API ATestCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATestCharacter();

    // 무기 장착 함수 (DataTableRowHandle 기반)
    void EquipWeaponFromRow(const FDataTableRowHandle& WeaponDataHandle);

    // 현재 캐릭터가 오버랩 중인 무기 액터 저장
    UPROPERTY()
    AActor* OverlappingPickUpActor = nullptr;

protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 카메라 관련 컴포넌트들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;

    // 이동 및 상호작용 함수
    UFUNCTION()
    void Move(const FInputActionValue& Value);

    UFUNCTION()
    void OnInteract(); // F 키 입력에 바인딩됨

    // 현재 장착 중인 무기
    UPROPERTY()
    AEquipWeaponMaster* EquippedWeapon;
};
