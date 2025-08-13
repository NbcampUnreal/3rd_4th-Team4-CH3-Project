#pragma once

#include "CoreMinimal.h"
#include "PppBaseAICharacter.h"
#include "Animation/AnimMontage.h"
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
    /*UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void ApplyMeleeDamage();*/

    // 근접 공격 애니메이션 (몽타주)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    UAnimMontage* MeleeAttackMontage;

    // 근접 공격 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float MeleeDamage = 10.0f; // 기본값 설정

    // AI가 근접 공격할 수 있는 최대 거리 (BT에 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float MeleeAttackRange = 200.0f;

    // 근접 공격 데미지 판정 시작 함수 (애니메이션 몽타주 노티파이에서 호출)
    UFUNCTION()
    void ApplyMeleeDamage();

    // 근접 공격 판정 콜리전 오버랩 이벤트 핸들러
    UFUNCTION()
    void OnMeleeDamageOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    // 근접 공격 판정 콜리전을 비활성화하는 함수
    UFUNCTION()
    void DeactivateMeleeDamageCollision();

    // 콜리전 활성화를 위한 타이머 핸들
    FTimerHandle MeleeCollisionTimerHandle;

    float StartTime=0;
protected:
    // 데미지 판정 범위를 나타내는 콜리전 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    USphereComponent* MeleeDamageCollision;
};
