#include "PPPCharacter.h"
#include "PppPlayerController.h"
#include "../Weapons/PickUpComponent.h"
#include "../Weapons/EquipWeaponMaster.h"
#include "../Weapons/WeaponRow.h"
#include "../Weapons/PickUpWeaponMaster.h"
#include "../GameMOde/PPPGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APppCharacter::APppCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

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
    FpsCameraComp->SetRelativeRotation(FRotator(-90.0f,0.0f,90.0f));
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
    bIsCameraChanged = false;
    OverlappingPickUpActor = nullptr;
    EquippedWeapon = nullptr;
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
             EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &APppCharacter::Move);

          if (PlayerController->PickUpAction)
              EnhancedInput->BindAction(PlayerController->PickUpAction, ETriggerEvent::Started, this, &APppCharacter::OnInteract);

          if (PlayerController->FireAction)
              EnhancedInput->BindAction(PlayerController->FireAction, ETriggerEvent::Triggered, this, &APppCharacter::Fire);

          if (PlayerController->JumpAction)
          {
             EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &APppCharacter::StartJump);
             EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &APppCharacter::StopJump);
          }

          if (PlayerController->LookAction)
             EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &APppCharacter::Look);

          if (PlayerController->SprintAction)
          {
             EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &APppCharacter::StartSprint);
             EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &APppCharacter::StopSprint);
          }

          if (PlayerController->CrouchAction)
          {
             EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Triggered, this, &APppCharacter::BeginCrouch);
             EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Completed, this, &APppCharacter::EndCrouch);
          }

          if (PlayerController->PovChangeAction)
             EnhancedInput->BindAction(PlayerController->PovChangeAction, ETriggerEvent::Triggered, this, &APppCharacter::ToggleCamera);
       }
    }
}

void APppCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return;
    const FVector2D MoveInput = value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(MoveInput.X))
        AddMovementInput(GetActorForwardVector(), MoveInput.X);

    if (!FMath::IsNearlyZero(MoveInput.Y))
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
}

void APppCharacter::StartJump(const FInputActionValue& value)
{
    if (value.Get<bool>())
        Jump();
}

void APppCharacter::StopJump(const FInputActionValue& value)
{
    if (!value.Get<bool>())
        StopJumping();
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
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APppCharacter::StopSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
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
    }
    else
    {
        FpsCameraComp->SetActive(false);
        TpsCameraComp->SetActive(true);
    }
}

float APppCharacter::GetHealth() const
{
    return CurrentHealth;
}

void APppCharacter::AddHealth(float Amount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
}

float APppCharacter::TakeDamage(
    float DamageAmount,
    struct FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

    if (CurrentHealth <= 0.0f)
        OnDeath();

    return ActualDamage;
}

void APppCharacter::OnDeath()
{
    APPPGameState* PPPGameState = GetWorld() ? GetWorld()->GetGameState<APPPGameState>() : nullptr;
    if (PPPGameState)
        OnCharacterDead.Broadcast();
}

// ----------- 무기 델리게이트 방식 ----------
void APppCharacter::OnInteract()
{
    if (OverlappingPickUpActor && OverlappingPickUpActor->PickUpComp)
    {
        OverlappingPickUpActor->PickUpComp->TryPickUp(this);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("F key pressed but no weapon overlapped"));
    }
}

void APppCharacter::Fire()
{
    if (EquippedWeapon)
        EquippedWeapon->Fire();
    else
        UE_LOG(LogTemp, Warning, TEXT("No weapon equipped!"));
}

void APppCharacter::DropWeaponToWorld(const FWeaponRow& StaticWeaponRow, FVector DropLocation, FRotator DropRotation)
{
    if (!StaticWeaponRow.PickUpWeapon)
        return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = nullptr;

    GetWorld()->SpawnActor<APickUpWeaponMaster>(
        StaticWeaponRow.PickUpWeapon,
        DropLocation,
        DropRotation,
        SpawnParams);
}
