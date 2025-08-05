#include "PppChaseAICharacter.h"
#include "PppChaseAIController.h"
#include "../Characters/PppCharacter.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h" // 애니메이션 몽타주 사용
#include "Components/SphereComponent.h"

APppChaseAICharacter::APppChaseAICharacter()
{
    // 이 캐릭터는 ChaseAIController에 의해 조종됨
    AIControllerClass = APppChaseAIController::StaticClass();
    // 월드에 배치되거나 스폰될 때 자동 Possess
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 근접 공격 데미지 판정용 콜리전 컴포넌트 생성
    MeleeDamageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeDamageCollision"));
    MeleeDamageCollision->SetupAttachment(RootComponent);
    MeleeDamageCollision->SetSphereRadius(MeleeAttackRange * 0.5f); // 적절한 크기 설정
    MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본적으로 비활성화
    MeleeDamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    MeleeDamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void APppChaseAICharacter::BeginPlay()
{
    Super::BeginPlay();
}

void APppChaseAICharacter::StartMeleeAttack()
{
    if (MeleeAttackMontage)
    {
        PlayAnimMontage(MeleeAttackMontage);
    }
}

void APppChaseAICharacter::ApplyMeleeDamage()
{
    // MeleeDamageCollision을 활성화하여 데미지 판정 시작
    MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // Overlap 이벤트를 감지하여 데미지 처리
    TArray<AActor*> OverlappingActors;
    MeleeDamageCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* OverlappingActor : OverlappingActors)
    {
        APppCharacter* PlayerCharacter = Cast<APppCharacter>(OverlappingActor);
        if (PlayerCharacter && PlayerCharacter != Cast<APppCharacter>(this))
        {
            UGameplayStatics::ApplyDamage(
                PlayerCharacter,
                MeleeDamage,
                GetController(),
                this,
                UDamageType::StaticClass()
            );

            // 한 번만 데미지를 적용하고 루프를 빠져나감
            break;
        }
    }

    // 데미지 판정이 끝나면 콜리전을 다시 비활성화
    MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
