// Copyright Epic Games, Inc. All Rights Reserved.

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
class AEquipWeaponMaster;//Test1 추가
struct FWeaponRow;//Test1 추가

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

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);
    //Test1 추가
    UPROPERTY()
    AActor* OverlappingPickUpActor = nullptr;
    //Test1 추가
    UPROPERTY()
    AEquipWeaponMaster* EquippedWeapon;
    //Test1 추가
    UFUNCTION()
    void OnInteract();
    //Test1 추가
    void EquipWeaponFromRow(const FDataTableRowHandle& WeaponDataHandle);
    //Test1 추가
    UFUNCTION()
    void Fire();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bIsCameraChanged;

	void OnDeath();


public:
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintAssignable)
    FOnCharacterDead OnCharacterDead;

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
};
