#include "WeaponTray.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "PppCharacter.h"
#include "EquipWeaponMaster.h"
#include "Engine/Texture2D.h"
#include "Engine/UserDefinedEnum.h"   // BP Enum 표시용

void UWeaponTray::NativeConstruct()
{
    Super::NativeConstruct();

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
    if (WeaponNameText != nullptr)
    {
        WeaponNameText->SetText(NewWeaponName);
    }

    // 두 슬롯 교차 채우기
    if (NewWeaponImage != nullptr)
    {
        if (bPrimaryNext && PrimaryWeaponImage != nullptr)
        {
            PrimaryWeaponImage->SetBrushFromTexture(NewWeaponImage);
        }
        else if (!bPrimaryNext && SecondaryWeaponImage != nullptr)
        {
            SecondaryWeaponImage->SetBrushFromTexture(NewWeaponImage);
        }
    }

    // 무기 교체 애니메이션
    if (WeaponSwap != nullptr)
    {
        PlayAnimation(WeaponSwap);
    }

    // 다음 채울 슬롯 토글
    bPrimaryNext = !bPrimaryNext;
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
    if (NewWeapon == nullptr)
    {
        // 비우기
        UpdateWeaponInfo(FText::GetEmpty(), nullptr);

        if (CurrentAmmoText)   CurrentAmmoText->SetText(FText::FromString(TEXT("0")));
        if (ReserveAmmoText)   ReserveAmmoText->SetText(FText::FromString(TEXT("0")));
        if (FireModeText)      FireModeText->SetText(FText::GetEmpty());
        return;
    }

    // 이름 우선순위: DisplayName 있으면 그거, 없으면 액터 이름
    const FText WeaponName =
        !NewWeapon->WeaponDisplayName.IsEmpty()
        ? NewWeapon->WeaponDisplayName
        : FText::FromName(NewWeapon->GetFName());

    UpdateWeaponInfo(WeaponName, NewWeapon->WeaponImage);
    UpdateFireModeTextFromWeapon(NewWeapon);

    // 탄약 초기 동기화(캐릭터에서도 브로드캐스트 해주지만 안전빵)
    HandleAmmoChanged(NewWeapon->CurrentAmmoInMag, NewWeapon->ReserveAmmo);
}

void UWeaponTray::HandleAmmoChanged(int32 InMag, int32 Reserve)
{
    UpdateAmmoText(InMag, Reserve);
}

void UWeaponTray::UpdateFireModeTextFromWeapon(AEquipWeaponMaster* Weapon)
{
    if (Weapon == nullptr || FireModeText == nullptr)
    {
        return;
    }

    // ── BP Enum(E_FireMode) DisplayName을 가져와 표기 ──
    // 무기 필드가 C++ enum이든 BP enum이든, 인덱스(uint8)로 읽어 매핑
    if (UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("E_FireMode"), true))
    {
        const int32 Index = static_cast<int32>(Weapon->FireMode); // Weapon->FireMode는 uint8/enum 기반
        const FText ModeLabel = EnumPtr->GetDisplayNameTextByIndex(Index);
        FireModeText->SetText(ModeLabel);
    }
    else
    {
        // 폴백
        FireModeText->SetText(FText::FromString(TEXT("Unknown")));
    }
}
