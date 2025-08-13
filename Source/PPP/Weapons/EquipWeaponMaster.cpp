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

    // 발사 지연 체크
    // WeaponFireDelay == 0 이면 기존 동작 유지(쿨다운 없음)
    if (WeaponFireDelay > 0.f)
    {
        const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
        const float Elapsed = Now - LastFireTime;

        if (Elapsed < WeaponFireDelay)
        {
            // 아직 대기시간이 남았으므로 차단
            // 남은 시간 로그 출력, 거슬리거나 신경쓰이면 삭제 가능
            UE_LOG(LogTemp, Verbose, TEXT("발사 지연 : 남은 %.3f초"), WeaponFireDelay - Elapsed);
            return;
        }
        // 통과하면 아래에서 정상 발사 처리 후 LastFireTime 갱신
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
    int32 SizeX = 0, SizeY = 0;
    PlayerController->GetViewportSize(SizeX, SizeY);
    const FVector2D ViewportCenter(SizeX / 2.f, SizeY / 2.f);

    FVector WorldLocation = FVector::ZeroVector;
    FVector WorldDirection = FVector::ForwardVector; // [ADD] 기본값 선언
    if (!PlayerController->DeprojectScreenPositionToWorld(ViewportCenter.X, ViewportCenter.Y, WorldLocation, WorldDirection))
    {
        UE_LOG(LogTemp, Warning, TEXT("화면 중심 좌표 디프로젝션 실패"));
        return;
    }

    if (PelletCount > 1)
    {
        for (int i = 0; i < PelletCount; ++i)
        {
            FVector PelletDir = FMath::VRandCone(WorldDirection, FMath::DegreesToRadians(SpreadAngle));
            FVector End = MuzzleLocation + (PelletDir * FireRange);

            FHitResult Hit;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);
            Params.AddIgnoredActor(GetOwner());

            FColor LineColor = FColor::Red;

            if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, End, ECC_Visibility, Params))
            {
                if (ApplyDamageToHit(Hit))
                {
                    LineColor = FColor::Green; // AI 맞춘 경우
                }
            }

            DrawDebugLine(GetWorld(), MuzzleLocation, End, LineColor, false, 1.0f, 0, 1.0f);
        }

        // 펠릿 발사 끝났으면 더 이상 아래 기본 발사 로직 실행 안 함
        --CurrentAmmoInMag;
        OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);
        LastFireTime = GetWorld()->GetTimeSeconds();
        return;
    }

    // 로켓런처: 폭발 판정
    if (ExplosionRadius > 0.f)
    {
        FVector End = MuzzleLocation + (WorldDirection * FireRange);

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);
        Params.AddIgnoredActor(GetOwner());

        if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, End, ECC_Visibility, Params))
        {
            UGameplayStatics::ApplyRadialDamage(
                GetWorld(),
                (ExplosionDamage > 0.f ? ExplosionDamage : Damage), // 반경 대신 데미지 사용
                Hit.ImpactPoint,
                ExplosionRadius,
                UDamageType::StaticClass(),
                {}, // 무시할 액터 배열
                this,
                OwnerCharacter->GetController(),
                true
            );

            //폭발 연출
            PlayExplosionEffect(Hit.ImpactPoint);
        }

        --CurrentAmmoInMag;
        OnAmmoChanged.Broadcast(CurrentAmmoInMag, ReserveAmmo);
        LastFireTime = GetWorld()->GetTimeSeconds();
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

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        if (ApplyDamageToHit(Hit))
        {
            LineColor = FColor::Green; // AI 맞춘 경우
        }
    }

    // 정현성 히트 마커와 킬 마커 두 개의 구분을 위해서는 AI가 맞고 죽었는지 혹은 맞고 살았는지 알아야하는데 데미지를 입힌 판정만 있어서 새로 써봅니다.
    // 피격 액터가 AI라면 데미지 적용
    //if (bHit && Hit.GetActor())
    //{
    //    APppBaseAICharacter* HitAI = Cast<APppBaseAICharacter>(Hit.GetActor());
    //    if (HitAI)
    //    {
    //        // 데미지 전달
    //        HitAI->TakeDamage(Damage, FDamageEvent(), nullptr, this);   // 또는 필요한 인자 대로
    //        LineColor = FColor::Green;
    //        UE_LOG(LogTemp, Warning, TEXT("AI를 피격하여 데미지를 입혔습니다."));
    //    }
    //    else
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("피격된 액터가 AI가 아닙니다."));
    //    }
    //}

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

    if (WeaponFireDelay > 0.f)
    {
        LastFireTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
    }
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
    // 샷건, 로켓 런처 데이터 캐싱
    PelletCount = InWeaponRow.PelletCount;
    SpreadAngle = InWeaponRow.SpreadAngle;
    ExplosionRadius = InWeaponRow.ExPlosionRadius;
    ExplosionDamage = InWeaponRow.ExplosionDamage;

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

    // 발사 지연 값 캐시(현재는 DT 값 사용, 추후 PDA 우선 필요 시 동일 패턴으로 확장
    WeaponFireDelay = InWeaponRow.FireDelay;

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
// 재장전 애니메이션 재생 함수 구현
void AEquipWeaponMaster::PlayReloadAnimation()
{
    if (SkeletalMesh && ReloadAnim)
    {
        SkeletalMesh->PlayAnimation(ReloadAnim, false);
    }
}

void AEquipWeaponMaster::PlayExplosionEffect(const FVector& Location)
{
    if (ExplosionFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, Location);
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, Location);
    }
}

bool AEquipWeaponMaster::ApplyDamageToHit(const FHitResult& Hit)
{
    // 먼저 Actor 포인터 얻기
    AActor* HitActor = Hit.GetActor();
    if (!HitActor) return false; // 맞은 액터 없으면 종료

    // 안전한 Actor 캐스팅 (컴포넌트가 아닌 Actor 대상)
    if (APppBaseAICharacter* HitAI = Cast<APppBaseAICharacter>(HitActor))
    {
        // 데미지 적용
        HitAI->TakeDamage(Damage, FDamageEvent(), nullptr, this);

        // 처치 여부 판정
        if (HitAI->GetHealth() <= 0.0f)
        {
            OnWeaponKilled.Broadcast();
            UE_LOG(LogTemp, Warning, TEXT("AI를 처치했습니다."));
        }
        else
        {
            OnWeaponHit.Broadcast();
            UE_LOG(LogTemp, Warning, TEXT("AI를 피격하여 데미지를 입혔습니다."));
        }

        return true; // AI 맞춤 → 초록색
    }
    else
    {
        // AI 타입이 아닌 경우
        UE_LOG(LogTemp, Log, TEXT("피격된 액터가 AI가 아닙니다."));
    }

    return false; // AI 아님 → 빨간색
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
