#include "PppBaseAICharacter.h"


APppBaseAICharacter::APppBaseAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    MaxHealth = 100.f;
    CurrentHealth = MaxHealth;
    Defense = 0.f;
    bIsDead = false;

    AutoPossessAI = EAutoPossessAI::Disabled;

    SightRadius = 1500.0f; // 기본 시야 반경
}

void APppBaseAICharacter::BeginPlay()
{
	Super::BeginPlay();

    CurrentHealth = MaxHealth;
    bIsDead = false;
}

float APppBaseAICharacter::TakeDamage(
    float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    // 실질 데미지 계산(방어력 적용, 최소 1)
    float NetDamage = FMath::Max(DamageAmount - Defense, 1.f);

    ApplyHealthChange(-NetDamage);

    // (Optional) 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("[%s] 피격! 받은 데미지: %.1f, 방어력: %.1f, 적용 데미지: %.1f, 현재 체력: %.1f"),
        *GetName(), DamageAmount, Defense, NetDamage, CurrentHealth);

    // 부모 함수 필요시 호출
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void APppBaseAICharacter::SetHealth(float NewHealth)
{
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth); // 체력은 0~MaxHealth로 범위 고정

    // 체력 변화가 있을 때만 델리게이트 호출
    if (CurrentHealth != OldHealth)
    {
        OnHealthChanged.Broadcast();  // 외부 시스템(UI, 이펙트 등과 연동 가능)
    }

    // 체력이 0 이하이면서 아직 죽지 않았다면 사망 처리
    if (CurrentHealth <= 0.f && !bIsDead)
    {
        Die(); // 별도로 구현한 사망 함수에서 OnDeath 델리게이트, 실제 Destroy 처리 등
    }
}

void APppBaseAICharacter::ApplyHealthChange(float Delta)
{
    SetHealth(CurrentHealth + Delta);
}

void APppBaseAICharacter::Die()
{
    if (!bIsDead)
    {
        bIsDead = true;
        OnDeath.Broadcast();
        Destroy();
    }
}

bool APppBaseAICharacter::IsDead() const
{
    return bIsDead;
}
