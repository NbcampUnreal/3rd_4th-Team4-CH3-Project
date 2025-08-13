#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PppBaseAICharacter.generated.h"

// // [델리게이트 선언]
// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

class UBehaviorTree;
class UAnimMontage; // 사망 애니메이션 몽타주를 위한 선언

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth); // 체력 변경 시 브로드캐스트할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath); // 사망 시 브로드캐스트할 델리게이트

UCLASS()
class PPP_API APppBaseAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APppBaseAICharacter();

    // 정현성
    // 히트마커 판정을 위해 체력 변수의 값을 반환하는 함수를 추가
    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealth() const;

protected:
	virtual void BeginPlay() override;

    private:
    UPROPERTY(VisibleAnywhere, Category = "Timer")
    bool bIsTimerRunning = false;


public:

    // 이 AI 캐릭터가 사용할 비헤이비어 트리 애셋
    UPROPERTY(EditAnywhere, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    // AI의 시야 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SightRadius = 1500.f;

    // 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    // 현재 체력
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth = 100.f;

    // 방어력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Defense = 10.f;

    // 캐릭터가 살아있는지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    bool bIsDead = false;

    // 체력 변경 시 호출될 델리게이트 (UI 업데이트 등)
    UPROPERTY(BlueprintAssignable, Category="Event")
    FOnHealthChanged OnHealthChanged;

    // 사망 시 호출될 델리게이트 (AI 로직 중단 등)
    UPROPERTY(BlueprintAssignable, Category="Event")
    FOnDeath OnDeath;

    // 사망 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DeathMontage;

    // 언리얼 엔진의 데미지 시스템을 오버라이드
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetHealth(float NewHealth);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void Die();

    UFUNCTION(BlueprintPure, Category = "Stats")
    bool IsDead() const;

protected:
    void ApplyHealthChange(float DeltaHealth);

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayDeathMontage();
};



