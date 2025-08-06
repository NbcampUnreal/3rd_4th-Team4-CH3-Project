#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PppBaseAICharacter.generated.h"

// [델리게이트 선언]
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

class UBehaviorTree;

UCLASS()
class PPP_API APppBaseAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APppBaseAICharacter();

protected:
	virtual void BeginPlay() override;

public:
    // 이 AI 캐릭터가 사용할 비헤이비어 트리 애셋
    UPROPERTY(EditAnywhere, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    // AI의 시야 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SightRadius;

// [체력 시스템 변수]
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float MaxHealth = 100.f;         // 최대 체력

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
    float CurrentHealth = 100.f;      // 현재 체력

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float Defense = 0.f;             // 방어력

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
    bool bIsDead = false;            // 사망 여부

    // [체력 시스템 델리게이트]
    UPROPERTY(BlueprintAssignable, Category="Event")
    FOnHealthChanged OnHealthChanged;   // 체력 변경시 호출

    UPROPERTY(BlueprintAssignable, Category="Event")
    FOnDeath OnDeath;                   // AI가 사망했을 때 호출

    // [체력 시스템 함수]
    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser
    ) override;   // 데미지를 입었을 때 호출

    UFUNCTION()
    void SetHealth(float NewHealth); // 새 체력값 적용

    UFUNCTION()
    void ApplyHealthChange(float Delta); // 체력 변화값 적용

    UFUNCTION()
    void Die();        // 사망 시 호출

    UFUNCTION(BlueprintPure)
    bool IsDead() const; // 사망 여부 반환


};
