#include "WeaponTray.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Kismet/GameplayStatics.h"
#include "PppCharacter.h"
#include "EquipWeaponMaster.h"
#include "Engine/Texture2D.h"
#include "PPP/GameMOde/GameDefines.h"

void UWeaponTray::SetHudVisible(bool bVisible)
{
    if (TrayAnchor)
    {
        TrayAnchor->SetVisibility
        (
            bVisible ? ESlateVisibility::SelfHitTestInvisible
                     : ESlateVisibility::Collapsed
        );
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
    if (CachedCharacter)
    {
        CachedCharacter->OnWeaponChanged.AddDynamic(this, &UWeaponTray::HandleWeaponChanged);
        CachedCharacter->OnAmmoChanged.AddDynamic(this, &UWeaponTray::HandleAmmoChanged);
    }
}

void UWeaponTray::NativeDestruct()
{
    CachedCharacter = Cast<APppCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (CachedCharacter)
    {
        CachedCharacter->OnWeaponChanged.AddDynamic(this, &UWeaponTray::HandleWeaponChanged);
        CachedCharacter->OnAmmoChanged.AddDynamic(this, &UWeaponTray::HandleAmmoChanged);
    }
    Super::NativeDestruct();
}

void UWeaponTray::UpdateWeaponInfo(const FText& NewWeaponName, UTexture2D* NewWeaponImage)
{
    // 이름만 갱신
    if (WeaponNameText)
    {
        WeaponNameText->SetText(NewWeaponName);
    }

    if (WeaponImage)
    {
        if (NewWeaponImage)
        {
            WeaponImage->SetBrushFromTexture(NewWeaponImage /*, true*/);
        }
        else
        {
            WeaponImage->SetBrushFromTexture(nullptr);
        }
    }

    // 애니메이션 재생은 선택
    // if (WeaponSwap && NewWeaponImage)
    //{
    //    PlayAnimation(WeaponSwap);
    //}
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

        if (CurrentAmmoText)  CurrentAmmoText->SetText(FText::FromString(TEXT("0")));
        if (ReserveAmmoText)  ReserveAmmoText->SetText(FText::FromString(TEXT("0")));
        if (FireModeText)     FireModeText->SetText(FText());

        if (WeaponImage)      WeaponImage->SetBrushFromTexture(nullptr);
        \
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
    UpdateFireModeTextFromWeapon(NewWeapon);
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

