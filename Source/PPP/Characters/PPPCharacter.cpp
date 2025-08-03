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
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->TargetArmLength = 100.f;
    SpringArmComp->SocketOffset = FVector(0.0f, 40.0f, 50.0f);

	TpsCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCameraComp"));
	TpsCameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	TpsCameraComp->bUsePawnControlRotation = false;

    FpsCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FpsCameraComp"));
    FpsCameraComp->SetupAttachment(GetMesh(), TEXT("head"));
    FpsCameraComp->SetRelativeLocation(FVector(0.0f, 10.0f, 0.0f));
    // 월드나 상대 회전이 명확해야 할 때는 SetRelativeRotation
    FpsCameraComp->SetRelativeRotation(FRotator(-90.0f,0.0f,90.0f));
    FpsCameraComp->SetRelativeScale3D_Direct(FVector(1.0f,1.0f,1.0f));
    FpsCameraComp->bUsePawnControlRotation = false;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -90.f),
		FQuat(FRotator(
			0.f, -90.f, 0.f)));


	NormalSpeed = 600.f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	bIsCrouched = false;
    bIsCameraChanged = false; // 카메라 시점 변경 값. 3인칭 시점이 기본
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
					ETriggerEvent::Triggered,
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

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
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
                    &APppCharacter::BeginCrouch
		        );

		        EnhancedInput->BindAction(
		            PlayerController->CrouchAction,
		            ETriggerEvent::Completed,
		            this,
		            &APppCharacter::EndCrouch
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

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
		UE_LOG(LogTemp, Warning, TEXT("Front or Back"));
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
		UE_LOG(LogTemp, Warning, TEXT("Side"));
	}
}
void APppCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
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
}
void APppCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Fast"));
	}
}
void APppCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Slow"));
	}
}

void APppCharacter::BeginCrouch(const FInputActionValue& value)
{
    Crouch();
}

void APppCharacter::EndCrouch(const FInputActionValue& value)
{
    UnCrouch();
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
