#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PPPGameInstance.generated.h"

// 맵 이동 사이에 유지할 최소 정보만 담는다(무기 클래스 등)
UCLASS()
class PPP_API UPPPGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:
    // 현재 장착 무기 클래스(맵 전환 후 재스폰용)
    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<AActor> SavedEquippedWeaponClass = nullptr;

    // 필요하면 탄약/강화수치 등도 여기에 추가 가능
    UPROPERTY(BlueprintReadWrite)
    int32 SavedAmmo = -1;

    UFUNCTION(BlueprintCallable)
    void SetEquippedWeaponClass(TSubclassOf<AActor> InClass) { SavedEquippedWeaponClass = InClass; }

    UFUNCTION(BlueprintCallable)
    TSubclassOf<AActor> GetEquippedWeaponClass() const { return SavedEquippedWeaponClass; }

    UPROPERTY(BlueprintReadWrite)
    int32 FinalScore = 0;
};
