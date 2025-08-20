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
    MeleeDamageCollision->SetupAttachment(GetMesh(), FName("hand_r"));
    MeleeDamageCollision->SetSphereRadius(25.0f);
    MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeleeDamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    MeleeDamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    MeleeDamageCollision->OnComponentBeginOverlap.AddDynamic(this, &APppChaseAICharacter::OnMeleeDamageOverlap);
    UE_LOG(LogTemp, Warning, TEXT("근접 공격 오버랩 이벤트가 바인딩되었습니다!"));

    // 임시 체력설정
    MaxHealth = 150.f;
    CurrentHealth = MaxHealth;
    Defense = 10.f;

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
    GetWorldTimerManager().ClearTimer(MeleeCollisionTimerHandle);
    // 충돌 감지 콜리전을 활성화
    MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    UE_LOG(LogTemp, Warning, TEXT("MeleeDamageCollision이 활성화되었습니다!"));
}

void APppChaseAICharacter::OnMeleeDamageOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("AI가 플레이어와 겹쳤습니다."));

    //float CurrentTime = GetWorld()->GetTimeSeconds();
    if (OtherActor && OtherActor != this)
    {

        // 플레이어 캐릭터인지 확인
        APppCharacter* PlayerCharacter = Cast<APppCharacter>(OtherActor);
        if (PlayerCharacter)
        {
            UE_LOG(LogTemp, Warning, TEXT("플레이어 캐릭터 찾기 성공!"));

            // 데미지 적용 로직
            MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            UGameplayStatics::ApplyDamage(
                PlayerCharacter,
                MeleeDamage,
                GetController(),
                this,
                UDamageType::StaticClass()
            );
            UE_LOG(LogTemp, Warning, TEXT("플레이어에게 피해 %f를 입혔습니다."), MeleeDamage);

        }
        else
        {
            // 플레이어도 아니고 AI도 아닌 다른 액터와 충돌했을 경우
            if (OtherActor)
            {
                UE_LOG(LogTemp, Warning, TEXT("플레이어 캐릭터 Cast 실패! 충돌 대상: %s"), *OtherActor->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("플레이어 캐릭터 Cast 실패! 충돌 대상이 유효하지 않습니다."));
            }
        }
    }
    GetWorldTimerManager().SetTimer(MeleeCollisionTimerHandle, this, &APppChaseAICharacter::ApplyMeleeDamage, 0.8f, false);
}

void APppChaseAICharacter::DeactivateMeleeDamageCollision()
{
    GetWorldTimerManager().ClearTimer(MeleeCollisionTimerHandle);
    MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    UE_LOG(LogTemp, Warning, TEXT("MeleeDamageCollision 비활성화."));
}
