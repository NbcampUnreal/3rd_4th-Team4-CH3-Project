#pragma once

#include "CoreMinimal.h"
#include "PppBaseAICharacter.h"
#include "PppChaseAICharacter.generated.h"

class USphereComponent;
class UAnimMontage;

UCLASS()
class PPP_API APppChaseAICharacter : public APppBaseAICharacter
{
	GENERATED_BODY()
public:
    APppChaseAICharacter();

protected:
    virtual void BeginPlay() override;

public:
    // 근접 공격 애니메이션을 시작하는 함수
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void StartMeleeAttack();

    // 실제 데미지를 적용하는 함수
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void ApplyMeleeDamage();

    // 근접 공격 애니메이션 (몽타주)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    UAnimMontage* MeleeAttackMontage;

    // 근접 공격 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float MeleeDamage = 10.0f; // 기본값 설정

    // AI가 근접 공격할 수 있는 최대 거리 (블랙보드 데코레이터에 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float MeleeAttackRange = 200.0f;

protected:
    // 데미지 판정 범위를 나타내는 콜리전 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Combat")
    USphereComponent* MeleeDamageCollision;
};
