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
    UE_LOG(LogTemp, Warning, TEXT("[Tray] Name=%s, Icon=%s, Prim=%d Sec=%d Ammo=%d"),
        *NewWeaponName.ToString(),
        *GetNameSafe(NewWeaponImage),
        PrimaryWeaponImage!=nullptr,
        SecondaryWeaponImage!=nullptr,
        AmmoImage!=nullptr
    );

    if (WeaponNameText)
    {
        WeaponNameText->SetText(NewWeaponName);
    }

    if (PrimaryWeaponImage)
    {
        PrimaryWeaponImage->SetBrushFromTexture(NewWeaponImage);
    }

    if (SecondaryWeaponImage)
    {
        SecondaryWeaponImage->SetBrushFromTexture(NewWeaponImage);
    }

    if (WeaponSwap && NewWeaponImage)
    {
        PlayAnimation(WeaponSwap);
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
        if (CurrentAmmoText)
        {
            CurrentAmmoText->SetText(FText::FromString(TEXT("0")));
        }
        if (ReserveAmmoText)
        {
            ReserveAmmoText->SetText(FText::FromString(TEXT("0")));
        }
        if (FireModeText)
        {
            FireModeText->SetText(FText());
        }

        // 잔상 제거
        if (PrimaryWeaponImage)
        {
            PrimaryWeaponImage->SetBrushFromTexture(nullptr);
        }

        // 여기서는 이전 아이콘도 지우는 게 안전함
        if (SecondaryWeaponImage)
        {
            SecondaryWeaponImage->SetBrushFromTexture(nullptr);
        }

        if (AmmoImage)
        {
            AmmoImage->SetBrushFromTexture(nullptr);
        }

        return;
    }

    // 무기 있음 → 표시
    bHasWeapon = true;
    SetHudVisible(true);

    // 직전 무기 아이콘을 Secondary에 먼저 채움
    if (SecondaryWeaponImage)
    {
        SecondaryWeaponImage->SetBrushFromTexture(LastIcon);
    }

    // 이름 계산
    const FText WeaponName =
        !NewWeapon->WeaponDisplayName.IsEmpty()
            ? NewWeapon->WeaponDisplayName
            : FText::FromName(NewWeapon->GetFName());

    // 현재 무기 아이콘 / 탄약 아이콘 / 발사모드 갱신하기
    UpdateWeaponInfo(WeaponName, NewWeapon->GetWeaponIcon());   // 무기 PNG
    SetAmmoIcon(NewWeapon->GetAmmoIcon());  // 탄약 PNG
    UpdateFireModeTextFromWeapon(NewWeapon);

    // 탄약 동기화
    HandleAmmoChanged(NewWeapon->CurrentAmmoInMag, NewWeapon->ReserveAmmo);

    // 다음 교체를 위해 현재 아이콘을 캐싱
    LastIcon = NewWeapon->GetWeaponIcon();
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
    if (!AmmoImage)
    {
        return; // null이면 아무것도 안해서 이전 이미지 남을 수 있으니 비워주기
    }
    if (NewAmmoImage)
    {
        AmmoImage->SetBrushFromTexture(NewAmmoImage);
    }
    else
    {
        // 아이콘 없을 때 잔상 제거
        AmmoImage->SetBrushFromTexture(nullptr);
    }
}
