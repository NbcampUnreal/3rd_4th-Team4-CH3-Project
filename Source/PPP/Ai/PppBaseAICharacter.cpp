#include "PppBaseAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // CharacterMovementComponent를 사용하기 위함
#include "Components/CapsuleComponent.h" // 캡슐 컴포넌트를 사용하기 위함 (충돌 비활성화 등)
#include "Animation/AnimMontage.h" // UAnimMontage 사용
#include "AIController.h" // AAIController 사용을 위해 포함
#include "BrainComponent.h"

APppBaseAICharacter::APppBaseAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    AutoPossessAI = EAutoPossessAI::Disabled;

    // 체력/방어력 시스템 초기화
    SightRadius = 1500.0f;
    MaxHealth = 100.0f;    // 기본 최대 체력
    CurrentHealth = MaxHealth; // 시작 시 현재 체력은 최대 체력과 동일
    Defense = 10.0f;       // 기본 방어력
    bIsDead = false;       // 시작 시 살아있는 상태
}

void APppBaseAICharacter::BeginPlay()
{
	Super::BeginPlay();


    // BeginPlay에서 현재 체력을 최대 체력으로 설정하여 안전성 확보
    CurrentHealth = MaxHealth;

    // 초기 체력 설정 시 델리게이트 브로드캐스트
    OnHealthChanged.Broadcast(CurrentHealth);
}
float APppBaseAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 이미 사망했거나 유효하지 않은 데미지라면 처리하지 않음
    if (bIsDead || DamageAmount <= 0.0f)
    {
        return 0.0f;
    }

    // 기본적으로 엔진의 데미지 처리 함수를 호출하여 필요한 기본 로직을 수행
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 방어력 적용 (간단하게 피해량에서 방어력만큼 감소)
    ActualDamage = FMath::Max(0.0f, ActualDamage - Defense);

    // 체력 감소
    ApplyHealthChange(-ActualDamage);

    // 잘 되는지 테스트용 UE_LOG
    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage. Current Health: %f / %f (after defense: %f)"), *GetName(), DamageAmount, CurrentHealth, MaxHealth, ActualDamage);

    return ActualDamage; // 최종 적용된 피해량 반환
}

void APppBaseAICharacter::SetHealth(float NewHealth)
{
    // 체력을 최대 체력 범위 내로 클램프
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

    if (CurrentHealth != OldHealth)
    {
        OnHealthChanged.Broadcast(CurrentHealth);
    }

    // 체력이 0이하가 되고 아직 사망하지 않았다면 사망 처리
    if (CurrentHealth <= 0.0f && !bIsDead)
    {
        Die();
    }
}

void APppBaseAICharacter::ApplyHealthChange(float DeltaHealth)
{
    SetHealth(CurrentHealth + DeltaHealth);
}

void APppBaseAICharacter::Die()
{
    if (bIsDead) return; // 이미 사망했다면 함수 종료
    bIsDead = true; // 사망 상태로 설정

    if (AAIController* AIController = Cast<AAIController>(GetController())) // AAIController로 캐스팅
    {
        AIController->StopMovement();
        if (AIController->GetBrainComponent())
        {
            AIController->GetBrainComponent()->StopLogic("Died"); // 비헤이비어 트리 중단 메시지
        }
        AIController->UnPossess(); // AI 컨트롤러와 캐릭터 연결 해제
    }

    // 충돌 비활성화 (캡슐 컴포넌트와 메쉬 모두)
    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCapsuleComponent()->SetGenerateOverlapEvents(false);
    }
    if (GetMesh())
    {
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메쉬 충돌도 비활성화
        GetMesh()->SetGenerateOverlapEvents(false);
    }

    // 기탁 델리게이트 호출로 GameMode에 통보
    if (OnDeath.IsBound())
    {
        OnDeath.Broadcast();
    }


    // 움직임 비활성화
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->DisableMovement();
        GetCharacterMovement()->StopMovementImmediately();
    }

    // 사망 애니메이션 재생
    PlayDeathMontage();

    // 사망 델리게이트 브로드캐스트
    OnDeath.Broadcast();

    UE_LOG(LogTemp, Warning, TEXT("%s has died!"), *GetName());

    // 일정 시간 후 액터 파괴
    SetLifeSpan(5.0f); // 5초 후 액터 파괴
}

void APppBaseAICharacter::PlayDeathMontage()
{
    if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance())
    {
        // 현재 재생 중인 모든 몽타주 중단
        GetMesh()->GetAnimInstance()->Montage_Stop(0.0f);
        GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage, 1.0f);
        UE_LOG(LogTemp, Log, TEXT("Playing Death Montage for %s"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DeathMontage is null or AnimInstance not found for %s. Attempting to set Ragdoll."), *GetName());
        // 사망 몽타주가 없으면 바로 물리 시뮬레이션 활성화
        if (GetMesh())
        {
            GetMesh()->SetCollisionProfileName(TEXT("Ragdoll")); // Ragdoll 물리 프로파일이 있다면 설정
            GetMesh()->SetSimulatePhysics(true);
        }
    }
}

bool APppBaseAICharacter::IsDead() const
{
    return bIsDead;
}

// 정현성
// GetHealth를 CurrentHealth에 맞추기

float APppBaseAICharacter::GetHealth() const
{
    return CurrentHealth;
}

