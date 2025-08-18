
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PppCharacterAnimInstance.generated.h"

UCLASS()
class PPP_API UPppCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
    virtual void NativeInitializeAnimation() override; // 정현수 애님 이니셜라이즈 생성
    virtual void NativeUpdateAnimation(float DeltaSeconds) override; // 정현수 애님 업데이트 생성

    // 정현수 애님에 이동 속도 반영
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    float GroundSpeed;

    // 정현수 애님 이동 방향값 반영
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveDirection;

    // 정현수 돌격 소총 애님 재생 결정
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsWeaponEquipped;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsReloading;
};
