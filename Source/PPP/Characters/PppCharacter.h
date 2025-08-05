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

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* TpsCameraComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FpsCameraComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bIsCameraChanged;

    UPROPERTY(BlueprintAssignable)
    FOnCharacterDead OnCharacterDead;

    // 무기 줍기/장착 시스템 필드들
    UPROPERTY()
    APickUpWeaponMaster* OverlappingPickUpActor;

    UPROPERTY()
    AEquipWeaponMaster* EquippedWeapon;

    UFUNCTION()
    void OnInteract();

    UFUNCTION()
    void Fire();

    void DropWeaponToWorld(const FWeaponRow& WeaponRow, FVector DropLocation, FRotator DropRotation);

    // 입력 및 캐릭터 조작
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual float TakeDamage(
       float DamageAmount,
       struct FDamageEvent const& DamageEvent,
       class AController* EventInstigator,
       AActor* DamageCauser) override;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Health")
    void AddHealth(float Amount);

    UFUNCTION()
    void Move(const FInputActionValue& value);
    UFUNCTION()
    void StartJump(const FInputActionValue& value);
    UFUNCTION()
    void StopJump(const FInputActionValue& value);
    UFUNCTION()
    void Look(const FInputActionValue& value);
    UFUNCTION()
    void StartSprint(const FInputActionValue& value);
    UFUNCTION()
    void StopSprint(const FInputActionValue& value);
    UFUNCTION()
    void BeginCrouch(const FInputActionValue& value);
    UFUNCTION()
    void EndCrouch(const FInputActionValue& value);
    UFUNCTION()
    void ToggleCamera();

    void OnDeath();

private:
    float NormalSpeed;
    float SprintSpeedMultiplier;
    float SprintSpeed;
    bool bIsCrouched;
};
