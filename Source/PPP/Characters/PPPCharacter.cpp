// Copyright Epic Games, Inc. All Rights Reserved.

#include "PppCharacter.h"
#include "PppPlayerController.h"
#include "../Weapons/EquipWeaponMaster.h"
#include "../Weapons/WeaponRow.h"
#include "../Weapons/PickUpWeaponMaster.h"
#include "Engine/DataTable.h"
#include "../GameMOde/PPPGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
APppCharacter::APppCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetMesh(), FName("Neck"));
	SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->TargetArmLength = 135.0f;
    SpringArmComp->SocketOffset = FVector(0.0f, 40.0f, 0.0f);

	TpsCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCameraComp"));
	TpsCameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	TpsCameraComp->bUsePawnControlRotation = false;

    FPsSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPsSpringArmComp"));
    FPsSpringArmComp->SetupAttachment(GetMesh(), FName("Head"));
    FPsSpringArmComp->TargetArmLength = -20.0f;
    FPsSpringArmComp->bUsePawnControlRotation = true;

    FpsCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FpsCameraComp"));
    FpsCameraComp->SetupAttachment(FPsSpringArmComp);
    FpsCameraComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    // 월드나 상대 회전이 명확해야 할 때는 SetRelativeRotation
    FpsCameraComp->SetRelativeRotation(FRotator(-90.0f,0.0f,90.0f));
    FpsCameraComp->bUsePawnControlRotation = false;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -90.f),
		FQuat(FRotator(
			0.f, -90.f, 0.f)));


	NormalSpeed = 250.f;
	SprintSpeedMultiplier = 4.0f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
    CrouchMovementSpeed = NormalSpeed / 50.0f;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

    CurrentWeaponIndex = 0;

	bIsCrouched = false;
    bIsCrouchKeyPressed = false;
    bIsCameraChanged = false; // 카메라 시점 변경 값. 3인칭 시점이 기본

    bIsZoomed = false;
}

// Called when the game starts or when spawned
void APppCharacter::BeginPlay()
{
	Super::BeginPlay();

    ToggleCamera();
}

// Called every frame
void APppCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APppCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (APppPlayerController* PlayerController = Cast<APppPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &APppCharacter::Move);
				UE_LOG(LogTemp, Warning, TEXT("YouCanMove"));
			}
            //Test1추가
		    if (PlayerController->PickUpAction)
		    {
		        EnhancedInput->BindAction(PlayerController->PickUpAction, ETriggerEvent::Started, this, &APppCharacter::OnInteract);
		    }
		    if (PlayerController->FireAction)
		    {
		        EnhancedInput->BindAction(PlayerController->FireAction, ETriggerEvent::Triggered, this, &APppCharacter::Fire);
		    }
		    // Test1추가 여기까지
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Started,
					this,
					&APppCharacter::StartJump
				);
				UE_LOG(LogTemp, Warning, TEXT("YouCanJump"));

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&APppCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&APppCharacter::Look
				);
			}

		    if (PlayerController->ZoomAction)
		    {
		        EnhancedInput->BindAction(
                    PlayerController->ZoomAction,
                    ETriggerEvent::Started,
                    this,
                    &APppCharacter::ZoomIn
                );

		        EnhancedInput->BindAction(
                    PlayerController->ZoomAction,
                    ETriggerEvent::Completed,
                    this,
                    &APppCharacter::ZoomOut
                );
		    }

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Started,
					this,
					&APppCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&APppCharacter::StopSprint
				);
			}

		    if (PlayerController->CrouchAction)
		    {
		        EnhancedInput->BindAction(
                    PlayerController->CrouchAction,
                    ETriggerEvent::Triggered,
                    this,
                    &APppCharacter::OnCrouchPressed
		        );

		        EnhancedInput->BindAction(
		            PlayerController->CrouchAction,
		            ETriggerEvent::Completed,
		            this,
		            &APppCharacter::OnCrouchReleased
		        );
		    }

		    if (PlayerController->PovChangeAction)
		    {
		        EnhancedInput->BindAction(
                    PlayerController->PovChangeAction,
                    ETriggerEvent::Triggered,
                    this,
                    &APppCharacter::ToggleCamera
                );
		    }
		}
	}
}

void APppCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

    if (bIsCrouchKeyPressed && !bIsCrouched)
    {
        return;
    }

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
		// UE_LOG(LogTemp, Warning, TEXT("Front or Back"));
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Side"));
	}
}
void APppCharacter::StartJump(const FInputActionValue& value)
{
    if (bIsCrouched) return;
	if (CanJump())
	{
		Jump();

		UE_LOG(LogTemp, Warning, TEXT("Jump"));
	}
}
void APppCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
		UE_LOG(LogTemp, Warning, TEXT("Landing"));
	}
}
void APppCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);

    // SpringArmComponent가 null이 아닐 경우에만 실행
    if (SpringArmComp || FPsSpringArmComp)
    {
        //FRotator CurrentRotation = SpringArmComp->GetRelativeRotation();
        if (SpringArmComp)
        {
            FRotator CurrentRotation = SpringArmComp->GetRelativeRotation();
            if (!FMath::IsNearlyZero(LookInput.Y))
            {
                //AddControllerPitchInput(LookInput.Y); 를 사용하지 않았을 때
                float PithRotation = LookInput.Y * GetWorld()->GetDeltaSeconds() * 100.f;
                PithRotation = CurrentRotation.Pitch - PithRotation;
                PithRotation = FMath::Clamp(PithRotation, -60.f, 80.f);
                CurrentRotation.Pitch = PithRotation;
                SpringArmComp->SetRelativeRotation(CurrentRotation);
                //UE_LOG(LogTemp, Warning, TEXT("회전"));
            }
        }

        else if (FPsSpringArmComp)
        {
            FRotator CurrentRotation = FPsSpringArmComp->GetRelativeRotation();
            if (!FMath::IsNearlyZero(LookInput.Y))
            {
                //AddControllerPitchInput(LookInput.Y); 를 사용하지 않았을 때
                float PithRotation = LookInput.Y * GetWorld()->GetDeltaSeconds() * 100.f;
                PithRotation = CurrentRotation.Pitch - PithRotation;
                PithRotation = FMath::Clamp(PithRotation, -40.f, 40.f);
                CurrentRotation.Pitch = PithRotation;
                SpringArmComp->SetRelativeRotation(CurrentRotation);
                //UE_LOG(LogTemp, Warning, TEXT("회전"));
            }
        }
        // IsNearlyZero : 거의 0인지에 대해 판별 (부동소수점 오차로 인해 완벽한 0이 아닐 수 있음)
        if (!FMath::IsNearlyZero(LookInput.X))
        {
            //AddControllerYawInput(LookInput.X); 를 사용하지 않았을 때
            // 마우스의 좌우 입력이 들어왔을 때
            float YawRotation = LookInput.X * GetWorld()->GetDeltaSeconds() * 100.f;
            AddActorLocalRotation(FRotator(0.0f, YawRotation, 0.0f)); // Pitch, Yaw, Roll
        }
    }
}
void APppCharacter::ZoomIn(const FInputActionValue& value)
{
    if (value.Get<bool>() == false)
    {
        if (FpsCameraComp)
        {
            FpsCameraComp->SetFieldOfView(45.0f);
        }
        UE_LOG(LogTemp, Warning, TEXT("Zoom In"));
        bIsZoomed = true;
    }
}

void APppCharacter::ZoomOut(const FInputActionValue& value)
{
    if (value.Get<bool>() == true)
    {
        if (FpsCameraComp)
        {
            FpsCameraComp->SetFieldOfView(90.0f);
        }
        UE_LOG(LogTemp, Warning, TEXT("Zoom Out"));
        bIsZoomed = false;
    }
}

void APppCharacter::StartSprint(const FInputActionValue& value)
{
    if (bIsCrouched) return;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Fast : %f"), GetCharacterMovement()->MaxWalkSpeed);
	}
}
void APppCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Slow : %f"), GetCharacterMovement()->MaxWalkSpeed);
	}
}

void APppCharacter::OnCrouchPressed(const FInputActionValue& value)
{
    bIsCrouchKeyPressed = true;
    // 아직 앉기 수행 아님
    UE_LOG(LogTemp, Warning, TEXT("Crouch Button Pressed"));
}

void APppCharacter::OnCrouchReleased(const FInputActionValue& value)
{
    bIsCrouchKeyPressed = false;

    // Crouch / UnCrouch 상태 전환 수행
    if (!bIsCrouched)
    {
        Crouch();
        bIsCrouched = true;

        if (GetCharacterMovement())
        {
            GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
        }
    }
    else
    {
        UnCrouch();
        bIsCrouched = false;

        if (GetCharacterMovement())
        {
            GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("CrouchEnd"));
}

void APppCharacter::ToggleCamera()
{
    bIsCameraChanged = !bIsCameraChanged;

    if (bIsCameraChanged)
    {
        FpsCameraComp->SetActive(true);
        TpsCameraComp->SetActive(false);
        UE_LOG(LogTemp, Warning, TEXT("FPSCamera"));
    }
    else
    {
        FpsCameraComp->SetActive(false);
        TpsCameraComp->SetActive(true);
        UE_LOG(LogTemp, Warning, TEXT("TPSCamera"));
    }
}

float APppCharacter::GetHealth() const
{
	return CurrentHealth;
}

void APppCharacter::AddHealth(float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health increased to: %f"), CurrentHealth);
}

float APppCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	// 실제 대미지는 입힌 데미지의 계산을 통해
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 최대 데미지의 범위가 체력의 최소 값의 범주를 벗어나지 않게 막음
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	//UpdateOverheadHP();
	UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), CurrentHealth);

	// 체력이 0이면 사망
	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void APppCharacter::OnDeath()
{
	APPPGameState* PPPGameState = GetWorld() ? GetWorld()->GetGameState<APPPGameState>() : nullptr;
	if (PPPGameState)
	{
	    OnCharacterDead.Broadcast();
	    UE_LOG(LogTemp, Warning, TEXT("You Died!"));
	}
}
// -------------------------------
// Test1추가 무기 장착
// -------------------------------
void APppCharacter::EquipWeaponFromRow(const FDataTableRowHandle& WeaponDataHandle)
{
    if (!WeaponDataHandle.DataTable || WeaponDataHandle.RowName.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid weapon data handle provided."));
        return;
    }

    const FWeaponRow* WeaponRow = WeaponDataHandle.GetRow<FWeaponRow>(TEXT("Equipping weapon from row"));

    if (WeaponRow && WeaponRow->EquipWeapon)
    {
        if (EquippedWeapon)
        {
            EquippedWeapon->Destroy();
            EquippedWeapon = nullptr;
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        AEquipWeaponMaster* NewWeapon = GetWorld()->SpawnActor<AEquipWeaponMaster>(
            WeaponRow->EquipWeapon,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParams
        );

        if (NewWeapon)
        {
            NewWeapon->OnEquipped(this, *WeaponRow);
            EquippedWeapon = NewWeapon;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find weapon data for row: %s"),*WeaponDataHandle.RowName.ToString());
    }
}
// -------------------------------
// Test1추가 상호작용 키
// -------------------------------
void APppCharacter::OnInteract()
{
    if (OverlappingPickUpActor)
    {
        APickUpWeaponMaster* Weapon = Cast<APickUpWeaponMaster>(OverlappingPickUpActor);
        if (Weapon)
        {
            Weapon->EquipWeapon();
            OverlappingPickUpActor = nullptr;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("F key pressed but no weapon overlapped"));
    }
}
void APppCharacter::Fire()
{
    UE_LOG(LogTemp, Warning, TEXT("빵야!"));

}
