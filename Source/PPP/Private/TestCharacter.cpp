#include "TestCharacter.h"
#include "TestPlayerController.h"
#include "EquipWeaponMaster.h"
#include "WeaponRow.h"
#include "EnhancedInputComponent.h"
#include "PickUpWeaponMaster.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/DataTable.h"

ATestCharacter::ATestCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;
}

void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (ATestPlayerController* PlayerController = Cast<ATestPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ATestCharacter::Move);
            }

            if (PlayerController->PickUpAction)
            {
                EnhancedInput->BindAction(PlayerController->PickUpAction, ETriggerEvent::Started, this, &ATestCharacter::OnInteract);
            }
        }
    }
}

void ATestCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;

    const FVector2D MoveInput = Value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
    }
}

void ATestCharacter::OnInteract()
{
    if (OverlappingPickUpActor)
    {
        APickUpWeaponMaster* Weapon = Cast<APickUpWeaponMaster>(OverlappingPickUpActor);
        if (Weapon)
        {
            Weapon->EquipWeapon();

            // 장착 후 오버랩 액터 초기화
            OverlappingPickUpActor = nullptr;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("F 키가 눌렸지만, 오버랩된 무기가 없습니다."));
    }
}

void ATestCharacter::EquipWeaponFromRow(const FDataTableRowHandle& WeaponDataHandle)
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
            FVector::ZeroVector, FRotator::ZeroRotator,
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
        UE_LOG(LogTemp, Error, TEXT("Failed to find weapon data for row: %s"), *WeaponDataHandle.RowName.ToString());
    }
}
