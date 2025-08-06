#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Engine/DataTable.h"
#include "PPPCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDead);

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class AEquipWeaponMaster;
struct FWeaponRow;
class APickUpWeaponMaster;
class UPickUpComponent;

UCLASS()
class PPP_API APppCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APppCharacter();

    /** 카메라 관련 */
    //카메라 스프링 암
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    USpringArmComponent* SpringArmComp;
    //FPS카메라 스프링 암
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    USpringArmComponent* FPsSpringArmComp;
    //TPS 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* TpsCameraComp;
    //FPS 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FpsCameraComp;
    //카메라 시점 변경 여부(FPS/TPS)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bIsCameraChanged;

    /** 체력 관련 */
    //캐릭터 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
    float MaxHealth;
    //캐릭터 현재 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
    float CurrentHealth;
    UFUNCTION(BlueprintCallable, Category = "Health")
    void AddHealth(float Amount);
    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealth() const;

    /** 무기 관련 */
    //현재 장착된 무기의 인덱스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int CurrentWeaponIndex;
    //줌 상태 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bIsZoomed;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    APickUpWeaponMaster* OverlappingPickUpActor = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AEquipWeaponMaster* EquippedWeapon;
    UFUNCTION()
    void OnInteract();

    void EquipWeaponFromRow(const FDataTableRowHandle& WeaponDataHandle);

    UFUNCTION()
    void Fire();
    UPROPERTY()
    int32 AttackDamage;

    /** 사망 관련 */
    UPROPERTY(BlueprintAssignable)
    FOnCharacterDead OnCharacterDead;

    //기본 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterSpeed")
    float NormalSpeed;
    //스프린트 배율
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterSpeed")
    float SprintSpeedMultiplier;

    void DropWeaponToWorld(const FWeaponRow& WeaponRow, FVector DropLocation, FRotator DropRotation);
protected:
	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser)
        override;

    /**입력 처리 */
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
    UFUNCTION()
    void ZoomIn(const FInputActionValue& value);
    UFUNCTION()
    void ZoomOut(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);
    UFUNCTION()
	void OnCrouchPressed(const FInputActionValue& value);
    UFUNCTION()
    void OnCrouchReleased(const FInputActionValue& value);
    UFUNCTION()
    void BeginCrouch(const FInputActionValue& value);
    UFUNCTION()
    void EndCrouch(const FInputActionValue& value);
    UFUNCTION()
    void ToggleCamera();
    void OnDeath();

public:
	virtual void Tick(float DeltaTime) override;


    UPROPERTY(BlueprintAssignable)
    FOnCharacterDead OnCharacterDead;

private:

	float SprintSpeed; //스프린트 속도
    float CrouchMovementSpeed;//앉은 상태 속도
    bool bIsCrouchKeyPressed;//crouch 키가 눌렸는지 여부

};
