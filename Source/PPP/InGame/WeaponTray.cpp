#include "WeaponTray.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Kismet/GameplayStatics.h"
#include "PppCharacter.h"
#include "EquipWeaponMaster.h"
#include "Engine/Texture2D.h"

void UWeaponTray::SetHudVisible(bool bVisible)
{
    const ESlateVisibility Vis =
        bVisible ? ESlateVisibility::SelfHitTestInvisible
                 : ESlateVisibility::Collapsed;

    if (TrayAnchor)
    {
        TrayAnchor->SetVisibility(Vis);
    }
    else
    {
        SetVisibility(Vis); // 루트로 폴백
    }
}

void UWeaponTray::NativeConstruct()
{
    Super::NativeConstruct();

    // 처음엔 무기 UI 숨김
    SetHudVisible(false);
    bHasWeapon = false;

    // 플레이어 캐릭터 캐싱
    CachedCharacter = Cast<APppCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (CachedCharacter != nullptr)
    {
        // 무기 변경 이벤트 바인드
        CachedCharacter->OnWeaponChanged.AddDynamic(this, &UWeaponTray::HandleWeaponChanged);

        // 탄약 변경 이벤트 바인드
        CachedCharacter->OnAmmoChanged.AddDynamic(this, &UWeaponTray::HandleAmmoChanged);
    }
}

void UWeaponTray::NativeDestruct()
{
    if (CachedCharacter != nullptr)
    {
        // 바인드 해제
        CachedCharacter->OnWeaponChanged.RemoveDynamic(this, &UWeaponTray::HandleWeaponChanged);
        CachedCharacter->OnAmmoChanged.RemoveDynamic(this, &UWeaponTray::HandleAmmoChanged);
        CachedCharacter = nullptr;
    }

    Super::NativeDestruct();
}

void UWeaponTray::UpdateWeaponInfo(const FText& NewWeaponName, UTexture2D* NewWeaponImage)
{
    if (WeaponNameText)
    {
        WeaponNameText->SetText(NewWeaponName);
    }

    if (NewWeaponImage)
    {
        if (bPrimaryNext && PrimaryWeaponImage)
        {
            PrimaryWeaponImage->SetBrushFromTexture(NewWeaponImage);
        }
        else if (SecondaryWeaponImage)
        {
            SecondaryWeaponImage->SetBrushFromTexture(NewWeaponImage);
        }

        if (WeaponSwap)
        {
            PlayAnimation(WeaponSwap);
        }
        bPrimaryNext = !bPrimaryNext;
    }
}


void UWeaponTray::UpdateAmmoText(int32 NewAmmoInMag, int32 NewReserveAmmo)
{
    if (CurrentAmmoText != nullptr)
    {
        CurrentAmmoText->SetText(FText::AsNumber(NewAmmoInMag));
    }

    if (ReserveAmmoText != nullptr)
    {
        ReserveAmmoText->SetText(FText::AsNumber(NewReserveAmmo));
    }
}

void UWeaponTray::HandleWeaponChanged(AEquipWeaponMaster* NewWeapon)
{
    if (!NewWeapon)
    {
        // 무기 없음 → 숨김 + 초기화
        bHasWeapon = false;
        SetHudVisible(false);

        UpdateWeaponInfo(FText::GetEmpty(), nullptr);
        if (CurrentAmmoText) CurrentAmmoText->SetText(FText::FromString(TEXT("0")));
        if (ReserveAmmoText) ReserveAmmoText->SetText(FText::FromString(TEXT("0")));
        if (FireModeText)   FireModeText->SetText(FText());

        // 잔상 제거
        if (PrimaryWeaponImage)   PrimaryWeaponImage->SetBrushFromTexture(nullptr);
        if (SecondaryWeaponImage) SecondaryWeaponImage->SetBrushFromTexture(nullptr);
        if (AmmoImage)            AmmoImage->SetBrushFromTexture(nullptr);
        return;
    }

    // 무기 있음 → 표시
    bHasWeapon = true;
    SetHudVisible(true);

    const FText WeaponName =
        !NewWeapon->WeaponDisplayName.IsEmpty()
            ? NewWeapon->WeaponDisplayName
            : FText::FromName(NewWeapon->GetFName());

    UpdateWeaponInfo(WeaponName, NewWeapon->GetWeaponIcon());
    SetAmmoIcon(NewWeapon->GetAmmoIcon());
    UpdateFireModeTextFromWeapon(NewWeapon);

    // 초기 탄약 동기화(안전빵)
    HandleAmmoChanged(NewWeapon->CurrentAmmoInMag, NewWeapon->ReserveAmmo);
}

void UWeaponTray::HandleAmmoChanged
(
    int32 InMag,
    int32 Reserve
)
{
    if (!bHasWeapon)
    {
        return; // 무기 없으면 무시
    }
    UpdateAmmoText(InMag, Reserve);
}


void UWeaponTray::UpdateFireModeTextFromWeapon
(
    AEquipWeaponMaster* Weapon
)
{
    if (!Weapon || !FireModeText)
    {
        return;
    }
    FireModeText->SetText(Weapon->GetFireModeText());
}

void UWeaponTray::SetAmmoIcon(UTexture2D* NewAmmoImage)
{
    if (AmmoImage && NewAmmoImage)
    {
        AmmoImage->SetBrushFromTexture(NewAmmoImage);
    }
}
