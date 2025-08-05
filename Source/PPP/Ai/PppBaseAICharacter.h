#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PppBaseAICharacter.generated.h"

class UBehaviorTree;
class UAniMontage; // 사망 애니메이션 몽타주를 위한 선언

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float); // 체력 변경 시 브로드캐스트할 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnDeath); // 사망 시 브로드캐스트할 델리게이트

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

    // 현재 체력
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    // 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth;

    // 방어력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Defense;

    // 캐릭터가 살아있는지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    bool bIsDead;

    // 체력 변경 시 호출될 델리게이트 (UI 업데이트 등)
    FOnHealthChanged OnHealthChanged;

    // 사망 시 호출될 델리게이트 (AI 로직 중단 등)
    FOnDeath OnDeath;


    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetHealth(float NewHealth);


    UFUNCTION(BlueprintCallable, Category = "Stats")
    void Die();


    UFUNCTION(BlueprintPure, Category = "Stats")
    bool IsDead() const { return bIsDead; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DeathMontage;

protected:

    void ApplyHealthChange(float DeltaHealth);


    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayDeathMontage();
};
