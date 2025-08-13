#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h" // 정현성 사용
#include "InputActionValue.h"
#include "Engine/DataTable.h"
#include "PPPCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDead);

// 정현성
// 체력 변경 델리게이트 체력 비율 0.0f ~ 1.0f
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealthPercentage);

// by Yeoul
// 무기 교체 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, class AEquipWeaponMaster*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, InMag, int32, Reserve);


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


    // 정현성
    // 체력 관련 Event Dispatcher 선언
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChangedSignature OnHealthChanged;


    // by Yeoul
    // 무기 변경 델리게이트
    // 블루프린트에서 바인드할 수 있게 공개 프로퍼티
    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnWeaponChanged OnWeaponChanged;
    /**
     *
     */
    UPROPERTY(BlueprintAssignable)
    FOnAmmoChanged OnAmmoChanged;

    // by Yeoul
    // 무기 장착 처리 함수
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetEquippedWeapon(AEquipWeaponMaster* NewWeapon);

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

    // 성준모, 장전 키 입력 시 호출될 함수
    UFUNCTION()
    void OnReload();


    UFUNCTION()
    bool GetIsRifleEquipped() const {return bIsRifleEquipped; }
    void EpuipRifle();
    void UnEquipRifle();

    // 정현성
    // 히트마커 위젯 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> HitMarkerWidgetClass;
    UPROPERTY()
    UUserWidget* HitMarkerWidget;

    // 정현성
    // 킬마커 위젯 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> KillMarkerWidgetClass;
    UPROPERTY()
    UUserWidget* KillMarkerWidget;

    // 정현성
    // 위젯 보임, 숨김 함수 선언
    UFUNCTION()
    void ShowHitMarker();
    UFUNCTION()
    void HideHitMarker();
    UFUNCTION()
    void ShowKillMarker();
    UFUNCTION()
    void HideKillMarker();


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

private:

	float SprintSpeed; //스프린트 속도
    float CrouchMovementSpeed;//앉은 상태 속도
    bool bIsCrouchKeyPressed;//crouch 키가 눌렸는지 여부

    bool bIsRifleEquipped; // 총기 애니메이션 적용

    // by Yeoul
    UFUNCTION()
    void OnWeaponAmmoChanged(int32 InMag, int32 Reserve);

    // 성준모, 재장전 애니메이션 동작 중인지 확인하는 부울 값
    bool bIsReloading = false;

    // 성준모, 장전 시간(타이머 핸들)
    FTimerHandle ReloadTimerHandle;

    // 성준모, 장전 완료 콜백
    UFUNCTION()
    void FinishReload();

    // 정현성
    // 위젯 타이머 설정
    FTimerHandle HitMarkerTimer;
    FTimerHandle KillMarkerTimer;

};
