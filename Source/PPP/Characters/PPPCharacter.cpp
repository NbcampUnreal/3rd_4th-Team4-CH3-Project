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


APppCharacter::APppCharacter()
{


	PrimaryActorTick.bCanEverTick = true;
    //TPS 카메라 스프링 암
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetMesh(), FName("Neck"));
	SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->TargetArmLength = 135.0f;
    SpringArmComp->SocketOffset = FVector(0.0f, 40.0f, 0.0f);

    TpsCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCameraComp"));
    TpsCameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    TpsCameraComp->bUsePawnControlRotation = false;
    // FPS 카메라 스프링 암
    FPsSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPsSpringArmComp"));
    FPsSpringArmComp->SetupAttachment(GetMesh(), FName("Head"));
    FPsSpringArmComp->TargetArmLength = -20.0f;
    FPsSpringArmComp->bUsePawnControlRotation = true;

    FpsCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FpsCameraComp"));
    FpsCameraComp->SetupAttachment(FPsSpringArmComp);
    FpsCameraComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    FpsCameraComp->SetRelativeRotation(FRotator(0.0f,0.0f,90.0f));// 월드나 상대 회전이 명확해야 할 때는 SetRelativeRotation
    FpsCameraComp->bUsePawnControlRotation = false;

    //캡슐, 메시 위치
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.f));
    GetMesh()->SetRelativeLocationAndRotation(
       FVector(0.f, 0.f, -90.f),
       FQuat(FRotator(
          0.f, -90.f, 0.f)));

    //캐릭터 속도 초기화
	NormalSpeed = 250.f;
	SprintSpeedMultiplier = 4.0f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
    CrouchMovementSpeed = NormalSpeed / 50.0f;

    //체력 초기화
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    //현재 무기 인덱스
    CurrentWeaponIndex = 0;

	bIsCrouched = false;
    bIsCrouchKeyPressed = false;
    bIsCameraChanged = false; // 카메라 시점 변경 값. 3인칭 시점이 기본
    bIsZoomed = false;

    OverlappingPickUpActor = nullptr;
    EquippedWeapon = nullptr;

    bIsReloading = false;
}

void APppCharacter::BeginPlay()
{
    Super::BeginPlay();
    ToggleCamera();

    // 정현성
    // 게임 시작 시 초기 체력 값을 블루프린트로 보냄
    OnHealthChanged.Broadcast(CurrentHealth / MaxHealth);

}

void APppCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APppCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (APppPlayerController* PlayerController = Cast<APppPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &APppCharacter::Move);

		    if (PlayerController->LookAction)
		        EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &APppCharacter::Look);

		    if (PlayerController->JumpAction)
		    {
		        EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &APppCharacter::StartJump);
		        EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &APppCharacter::StopJump);
		    }
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
		    if (PlayerController->ZoomAction)
		    {
		        EnhancedInput->BindAction(PlayerController->ZoomAction,ETriggerEvent::Started,this,&APppCharacter::ZoomIn);
		        EnhancedInput->BindAction(PlayerController->ZoomAction,ETriggerEvent::Completed,this,&APppCharacter::ZoomOut);
		    }
		    if (PlayerController->PickUpAction)
		        EnhancedInput->BindAction(PlayerController->PickUpAction, ETriggerEvent::Started, this, &APppCharacter::OnInteract);

		    if (PlayerController->FireAction)
		        EnhancedInput->BindAction(PlayerController->FireAction, ETriggerEvent::Triggered, this, &APppCharacter::Fire);

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
             EnhancedInput->BindAction(PlayerController->PovChangeAction, ETriggerEvent::Triggered, this, &APppCharacter::ToggleCamera);

		    if (PlayerController->ReloadAction)
		    {
		        EnhancedInput->BindAction(PlayerController->ReloadAction, ETriggerEvent::Started, this, &APppCharacter::OnReload);
		        UE_LOG(LogTemp, Warning, TEXT("[Check] ReloadAction bound in character"));
		    }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[Issue] ReloadAction is NULL. Assign IA_Reload in PlayerController."));
            }
       }
    }
}

void APppCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return;

    const FVector2D MoveInput = value.Get<FVector2D>();

    if (bIsCrouchKeyPressed && !bIsCrouched)
        return;

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
        StopJumping();
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
                FPsSpringArmComp->SetRelativeRotation(CurrentRotation); //기탁 테스트 수정
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
    }
    else
    {
        FpsCameraComp->SetActive(false);
        TpsCameraComp->SetActive(true);
    }
}

// by Yeoul
// 무기 변경 델리게이트
void APppCharacter::SetEquippedWeapon(AEquipWeaponMaster* NewWeapon)
{
    // 현재 무기와 새로 지정하려는 무기가 같으면 아무것도 안 함
    if (EquippedWeapon == NewWeapon)
    {
        return; // 중복 변경 방지
    }

    // 기존 무기 델리게이트 언바인드
    if (EquippedWeapon != nullptr)
    {
        EquippedWeapon->OnAmmoChanged.RemoveDynamic(this, &APppCharacter::OnWeaponAmmoChanged);
    }

    // 교체
    EquippedWeapon = NewWeapon;

    // 무기 변경 알림
    OnWeaponChanged.Broadcast(NewWeapon);

    // 새 무기 델리게이트 바인드
    if (EquippedWeapon != nullptr)
    {
        EquippedWeapon->OnAmmoChanged.AddDynamic(this, &APppCharacter::OnWeaponAmmoChanged);

        // 즉시 현재 탄약 상태를 UI에 반영 (안전빵)
        OnWeaponAmmoChanged(EquippedWeapon->CurrentAmmoInMag,
                            EquippedWeapon->ReserveAmmo);
    }
    else
    {
        // 맨손 등
        OnWeaponAmmoChanged(0, 0);
    }
}

/**무기 관련 */
void APppCharacter::OnInteract()
{
    UE_LOG(LogTemp, Warning, TEXT("OnInteract() 호출됨"));

    if (OverlappingPickUpActor && OverlappingPickUpActor->PickUpComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("OverlappingPickUpActor 존재, TryPickUp 실행"));
        OverlappingPickUpActor->PickUpComp->TryPickUp(this);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("F key pressed but no weapon overlapped"));
    }
}

void APppCharacter::Fire()
{
    // 재장전 중에는 발사 금지
    if (bIsReloading)
    {
        UE_LOG(LogTemp, Verbose, TEXT("사격 불가: 재장전 중"));
        return;
    }

    // 발사
    if (EquippedWeapon)
    {
        EquippedWeapon->Fire();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No weapon equipped!"));
    }
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

    UE_LOG(LogTemp, Warning, TEXT("빵야!"));
}
/**체력 관련 */
float APppCharacter::GetHealth() const
{
    return CurrentHealth;
}

void APppCharacter::AddHealth(float Amount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);

    // 정현성
    // 체력이 변경될 때마다 Event Dispatcher 호출
    float HealthPercentage = CurrentHealth / MaxHealth;
    OnHealthChanged.Broadcast(HealthPercentage);


}

float APppCharacter::TakeDamage(
    float DamageAmount,
    struct FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

    // 정현성
    // 체력이 변경될 때마다 Event Dispatcher 호출
    float HealthPercentage = CurrentHealth / MaxHealth;
    OnHealthChanged.Broadcast(HealthPercentage);

    if (CurrentHealth <= 0.0f)
        OnDeath();

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

//오류 수정
void APppCharacter::BeginCrouch(const FInputActionValue& Value)
{
    Crouch();
}

void APppCharacter::EndCrouch(const FInputActionValue& Value)
{
    UnCrouch();
}

// by Yeoul
// 탄약 변경 이벤트 콜백 구현
void APppCharacter::OnWeaponAmmoChanged(int32 CurrentAmmoInMag, int32 ReserveAmmo)
{
    // 탄약 변경 이벤트 발생
    OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);
    UE_LOG(LogTemp, Warning, TEXT("탄약 변경: 현재 탄창 %d, 예비 탄약 %d"), CurrentAmmoInMag, ReserveAmmo);
}

// 성준모, 장전 입력 시 호출되는 함수 구현
void APppCharacter::OnReload()
{
    UE_LOG(LogTemp, Warning, TEXT("[Check] OnReload pressed"));

    // 장착된 무기가 없을 때 실행
    if (!EquippedWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("Reload 실패 : 장착된 무기가 없습니다."));
        return;
    }

    // 재장전 중복 방지
    if (bIsReloading)
    {
        // Verbose : 상세 로그 레벨로, 매우 세부적인 진단용 메세지.
        UE_LOG(LogTemp, Verbose, TEXT("이미 재장전 중입니다."));
        return;
    }

    // 데이터 테이블에서 온 ReloadTime 사용
    float ReloadTime = EquippedWeapon->WeaponDataRow.ReloadTime;

    bIsReloading = true;  // 핵심 함수, 무기 재장전 (무기 Reload 호출)

    // 유효한 장전 시간이면 타이머, 아니면 즉시 완료
    if (ReloadTime > 0.f)
    {
        GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &APppCharacter::FinishReload, ReloadTime, false);
    }
    else
    {
        // 장전 시간이 0이거나 잘못된 경우 즉시 완료 처리
        FinishReload();
    }
}

// 성준모, 타이머 완료 시 호출되어 실제 탄약을 채우는 함수
void APppCharacter::FinishReload()
{
    // 타이머 초기화
    GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

    // 장착된 무기가 없을 때 실행
    if (!EquippedWeapon)
    {
        bIsReloading = false;
        UE_LOG(LogTemp, Warning, TEXT("재장전 실패: 무기가 해제되었습니다."));
        return;
    }

    // 실제 탄창 채우기, OnAmmoChanged 브로드 캐스트 까지 수행
    EquippedWeapon->Reload();

    bIsReloading = false;
    UE_LOG(LogTemp, Log, TEXT("재장전 완료"));
}

