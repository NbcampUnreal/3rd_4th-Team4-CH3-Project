#include "EquipWeaponMaster.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Characters/PppCharacter.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "PPP/Ai/PppBaseAICharacter.h"
#include "WeaponRow.h"  // by Yeoul: FWeaponRow 구조체 사용
#include "WeaponTypes.h"    // by Yeoul: EFireMode Enum 사용
#include "../InGame/WeaponDataAsset.h"  // by Yeoul: 무기 데이터 에셋 사용

AEquipWeaponMaster::AEquipWeaponMaster()
{
    PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(Scene);

    // by Yeoul
    CurrentAmmoInMag = 0;
    ReserveAmmo = 0;
}

// 발사 절차:
// - 소유 캐릭터/플레이어컨트롤러 확인
// - 총구 소켓 위치 획득 후 화면 중앙 디프로젝션 방향으로 라인트레이스
// - 피격 시 AI 데미지 처리, 디버그 라인 색상 변경
// - 발사 애니메이션 재생 및 OnWeaponFired 이벤트 브로드캐스트
void AEquipWeaponMaster::Fire()
{
    // by Yeoul
    // 탄약 없으면 발사 중단
    if (CurrentAmmoInMag <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("탄약이 없습니다!"));
        return;
    }

    APppCharacter* OwnerCharacter = Cast<APppCharacter>(GetOwner());
    if (!OwnerCharacter) return;

    APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PlayerController) return;

    // 발사 애니메이션 재생
    if (SkeletalMesh && FireAnim)
    {
        SkeletalMesh->PlayAnimation(FireAnim, false);
    }

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

    // 화면 중앙 좌표 → 월드 위치/방향으로 디프로젝션
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

    // 총구에서 화면 중앙 방향(FireRange만큼)으로 라인트레이스
    FVector Start = MuzzleLocation;
    FVector End = Start + (WorldDirection * FireRange);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    // 기본 라인 색상: 적중 시 Green, 미적중/비AI 시 Red
    FColor LineColor = FColor::Red;

    // 디버깅: 피격 컴포넌트 정보 출력
    if (bHit && Hit.Component.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("피격된 컴포넌트 이름 : %s"), *Hit.Component->GetName());
        UE_LOG(LogTemp, Warning, TEXT("피격된 컴포넌트 클래스 : %s"), *Hit.Component->GetClass()->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("피격된 것 없음"));
    }

    // 피격 액터가 AI라면 데미지 적용
    if (bHit && Hit.GetActor())
    {
        APppBaseAICharacter* HitAI = Cast<APppBaseAICharacter>(Hit.GetActor());
        if (HitAI)
        {
            // 데미지 전달
            HitAI->TakeDamage(Damage, FDamageEvent(), nullptr, this);   // 또는 필요한 인자 대로
            LineColor = FColor::Green;
            UE_LOG(LogTemp, Warning, TEXT("AI를 피격하여 데미지를 입혔습니다."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("피격된 액터가 AI가 아닙니다."));
        }
    }

    // 시각적 디버그 라인, 라인 트레이스 레이저 시각화
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

    // by Yeoul
    // 실제 발사 성공 시 탄약 1 감소 -> 이벤트 송출
    -- CurrentAmmoInMag;

    // 발사 이벤트 브로드캐스트(히트 결과 전달)
    OnWeaponFired.Broadcast(Hit);

    // by Yeoul
    // 탄약 변경 이벤트 -> UI 갱신
    OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);
}

// 장착 처리:
// - WeaponRow에서 스탯 세팅
// - 오너 캐릭터의 손 소켓(hand_r)에 부착(Snap)
// - 무기별 위치/회전 오프셋 적용
// - 오너 설정 및 로그 출력
void AEquipWeaponMaster::OnEquipped(APppCharacter* NewOwner, const FWeaponRow& InWeaponRow)
{
    // by Yeoul
    // SAFE: 오너 체크
    if (!NewOwner)
    {
        return;
    }

    // by Yeoul
    // DT → PDA 캐시
    CachedData = InWeaponRow.WeaponData.IsValid()
        ? InWeaponRow.WeaponData.Get()
        : InWeaponRow.WeaponData.LoadSynchronous();

    Damage = InWeaponRow.Damage;
    MagazineSize = InWeaponRow.MagazineSize;
    ReloadTime = InWeaponRow.ReloadTime;
    // WeaponName = InWeaponRow.WeaponName;
    FireRange = InWeaponRow.FireRange;
    WeaponDataRow = InWeaponRow;
    WeaponIndex = InWeaponRow.WeaponIndex;

    // by Yeoul
    // 표시 이름(FText) : PDA가 있으면 PDA로, 없으면 DT로 (FName→FText)
    WeaponDisplayName = (CachedData && !CachedData->WeaponName.IsEmpty())
        ? CachedData->WeaponName
        : FText::FromName(InWeaponRow.WeaponName);

    // by Yeoul
    // PDA 우선 덮어쓰기(있을 때만)
    if (CachedData)
    {
        if (CachedData->MaxMagSize > 0)
        {
            MagazineSize = CachedData->MaxMagSize;
        }
        if (CachedData->WeaponThumbnail)
        {
            WeaponImage = CachedData->WeaponThumbnail;
        }
        if (CachedData->AmmoThumbnail)
        {
            AmmoImage = CachedData->AmmoThumbnail;
        }
        // FireMode 사용은 GetFireMode()에서 PDA 참조
    }

    AttachToComponent(
       NewOwner->GetMesh(),
       FAttachmentTransformRules::SnapToTargetNotIncludingScale,
       FName(TEXT("hand_r"))
   );

    // 무기별 위치, 회전 오프셋(데이터테이블 설정값) 적용
    SetActorRelativeLocation(InWeaponRow.WeaponOffset);
    SetActorRelativeRotation(InWeaponRow.WeaponRotation);

    SetOwner(NewOwner);

    // by Yeoul
    // 탄약 초기화 + 브로드캐스트
    CurrentAmmoInMag = MagazineSize;
    ReserveAmmo      = InWeaponRow.ReserveAmmo;
    OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);

    UE_LOG(LogTemp, Warning, TEXT("무기 장착 완료: %s"), *WeaponDisplayName.ToString());  // by Yeoul: FText 사용
}

// 드랍 처리:
// - 부모 분리(월드 트랜스폼 유지), 소유자 해제
// - 물리 시뮬레이션/중력 활성화로 바닥으로 떨어지게 함
// - 드랍 이벤트 브로드캐스트
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

// by Yeoul
// 재장전 함수
void AEquipWeaponMaster::Reload()
{
    // PDA > DT > 멤버 순으로 최대 탄창 크기 결정
    const int32 MaxAmmoInMag =
        (CachedData && CachedData->MaxMagSize > 0) ? CachedData->MaxMagSize :
        (WeaponDataRow.MagazineSize > 0)           ? WeaponDataRow.MagazineSize :
                                                     MagazineSize;

    const int32 AmmoNeeded = MaxAmmoInMag - CurrentAmmoInMag;

    // 예비탄이 없거나 이미 가득 차면 재장전 불필요 → 즉시 종료
    if (ReserveAmmo <= 0 || AmmoNeeded <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("재장전할 탄약이 없거나, 탄창이 이미 가득 찼습니다."));
        OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);
        return;
    }

    const int32 Use = FMath::Min(ReserveAmmo, AmmoNeeded);
    CurrentAmmoInMag += Use;
    ReserveAmmo      -= Use;

    // 탄약 변경 델리게이트 호출
    OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);
}

// by Yeoul
EFireMode AEquipWeaponMaster::GetFireMode() const
{
    return CachedData ? CachedData->FireMode : EFireMode::Single;
}

UTexture2D* AEquipWeaponMaster::GetWeaponIcon() const
{
    return CachedData ? CachedData->WeaponThumbnail : WeaponImage;
}

UTexture2D* AEquipWeaponMaster::GetAmmoIcon() const
{
    return CachedData ? CachedData->AmmoThumbnail : AmmoImage; // PDA 우선
}

UWeaponDataAsset* AEquipWeaponMaster::GetWeaponData() const
{
    return CachedData;
}


FText AEquipWeaponMaster::GetFireModeText() const
{
    switch (GetFireMode())
    {
    case EFireMode::Single: return NSLOCTEXT("Weapon", "Single", "Single");
    case EFireMode::Auto:   return NSLOCTEXT("Weapon", "Auto",   "Auto");
    default:                return NSLOCTEXT("Weapon", "Unknown","Unknown");
    }
}
