#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponTray.generated.h"

/**
 * by Yeoul
 * @class UWeaponTray
 * @brief 무기 트레이(Weapon Tray)를 나타내는 클래스입니다.
 *
 * UWeaponTray 클래스는 무기 정보를 UI 위젯으로 나타내기 위해 사용됩니다.
 * 주로 Unreal Engine 4/5의 UMG(User Interface Module) 기반으로 만들어졌으며,
 * 무기와 관련된 사용자 인터페이스를 관리합니다.
 */

// 클래스 전방 선언
class UTextBlock;
class UImage;
class UWidgetAnimation;
class UWidget;
class UTexture2D;
class APppCharacter;
class AEquipWeaponMaster;

UCLASS(BlueprintType, Blueprintable)
class PPP_API UWeaponTray : public UUserWidget
{
    GENERATED_BODY()

public:
    // 위젯 생성 시 호출되는 함수 (델리게이트 바인드)
    virtual void NativeConstruct() override;
    // 위젯 파괴 시 호출되는 함수 (델리게이트 해제)
    virtual void NativeDestruct() override;

    // 위젯이 보이는지 여부를 설정하는 함수
    UFUNCTION(BlueprintCallable, Category="UI")
    void SetHudVisible(bool bVisible);

    // 무기 정보를 업데이트 하는 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateWeaponInfo(const FText& NewWeaponName, UTexture2D* NewWeaponImage);

    // 탄약 정보를 업데이트 하는 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateAmmoText(int32 NewAmmoInMag, int32 NewReserveAmmo);

protected:
    // 블루프린트 위젯 바인딩
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> WeaponNameText;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> PrimaryWeaponImage;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> SecondaryWeaponImage;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentAmmoText;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ReserveAmmoText;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> FireModeText;
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UImage> AmmoImage;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UWidget> TrayAnchor;

    // 애니메이션 바인딩
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* WeaponSwap;

private:
    // 델리게이트 바인드용
    UFUNCTION()
    void HandleWeaponChanged(AEquipWeaponMaster* NewWeapon);

    UFUNCTION()
    void HandleAmmoChanged(int32 InMag, int32 Reserve);

    // Fire Mode UI 갱신
    void UpdateFireModeTextFromWeapon(AEquipWeaponMaster* Weapon);

    // 아이콘 세터
    UFUNCTION(BlueprintCallable, Category="UI")
    void SetAmmoIcon(UTexture2D* NewAmmoImage);

private:
    // 캐릭터 참조를 저장
    UPROPERTY() APppCharacter* CachedCharacter;
    // 이미지 번갈아 표시
    bool bPrimaryNext = true;

    // 무기 보유 여부 가드
    bool bHasWeapon = false;
};
