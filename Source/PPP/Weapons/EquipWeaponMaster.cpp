#include "EquipWeaponMaster.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Characters/PppCharacter.h"
#include "DrawDebugHelpers.h"
#include "PPP/Ai/PppBaseAICharacter.h"

AEquipWeaponMaster::AEquipWeaponMaster()
{
    PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(Scene);
}

void AEquipWeaponMaster::Fire()
{
    APppCharacter* OwnerCharacter = Cast<APppCharacter>(GetOwner());
    if (!OwnerCharacter) return;

    APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PlayerController) return;

    FVector MuzzleLocation = FVector::ZeroVector;
    if (SkeletalMesh && SkeletalMesh->DoesSocketExist(FName("MuzzleFlash")))
    {
        MuzzleLocation = SkeletalMesh->GetSocketLocation(FName("MuzzleFlash"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("총구 소켓이 존재하지 않습니다!"));
        return;
    }

    int32 SizeX, SizeY;
    PlayerController->GetViewportSize(SizeX, SizeY);
    FVector2D ViewportCenter(SizeX / 2.f, SizeY / 2.f);

    FVector WorldLocation, WorldDirection;
    if (!PlayerController->DeprojectScreenPositionToWorld(
        ViewportCenter.X, ViewportCenter.Y, WorldLocation, WorldDirection))
    {
        UE_LOG(LogTemp, Warning, TEXT("화면 중심 좌표 디프로젝션 실패"));
        return;
    }

    FVector Start = MuzzleLocation;
    FVector End = Start + (WorldDirection * FireRange);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    FColor LineColor = FColor::Red;

    if (bHit && Hit.Component.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("피격된 컴포넌트 이름 : %s"), *Hit.Component->GetName());
        UE_LOG(LogTemp, Warning, TEXT("피격된 컴포넌트 클래스 : %s"), *Hit.Component->GetClass()->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("피격된 것 없음"));
    }

    if (bHit && Hit.GetActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("피격된 액터 이름 : %s"), *Hit.GetActor()->GetName());
        UE_LOG(LogTemp, Warning, TEXT("피격된 액터 클래스 이름 : %s"), *Hit.GetActor()->GetClass()->GetName());

        if (Hit.GetActor()->IsA(APppBaseAICharacter::StaticClass()))
        {
            UE_LOG(LogTemp, Warning, TEXT("AI 피격했습니다 !!! 레이저 선이 초록색으로 바뀝니다 !!!"));
            LineColor = FColor::Green;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("피격된 액터가 AI가 아닙니다."));
        }
    }

    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        LineColor,
        false,
        1.0f,
        0,
        2.0f
    );

    OnWeaponFired.Broadcast(Hit);
}

void AEquipWeaponMaster::OnEquipped(APppCharacter* NewOwner, const FWeaponRow& InWeaponRow)
{
    Damage = InWeaponRow.Damage;
    MagazineSize = InWeaponRow.MagazineSize;
    ReloadTime = InWeaponRow.ReloadTime;
    WeaponName = InWeaponRow.WeaponName;
    FireRange = InWeaponRow.FireRange;
    WeaponDataRow = InWeaponRow;

    AttachToComponent(
       NewOwner->GetMesh(),
       FAttachmentTransformRules::SnapToTargetNotIncludingScale,
       FName(TEXT("hand_r"))
   );

    // [수정된 부분] 각 무기별 위치/회전 오프셋 적용
    SetActorRelativeLocation(InWeaponRow.WeaponOffset);
    SetActorRelativeRotation(InWeaponRow.WeaponRotation);

    SetOwner(NewOwner);

    UE_LOG(LogTemp, Warning, TEXT("무기 장착 완료: %s"), *InWeaponRow.WeaponName.ToString());
}

void AEquipWeaponMaster::Drop()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetOwner(nullptr);

    if (SkeletalMesh)
    {
        SkeletalMesh->SetSimulatePhysics(true);
        SkeletalMesh->SetEnableGravity(true);
    }

    OnWeaponDropped.Broadcast(this);
}
