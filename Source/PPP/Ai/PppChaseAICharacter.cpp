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
    MeleeDamageCollision->SetupAttachment(GetMesh(), FName("hand_r")); // 여기에 원하는 소켓 이름으로 변경
    MeleeDamageCollision->SetSphereRadius(25.0f);
    MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeleeDamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    MeleeDamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    MeleeDamageCollision->OnComponentBeginOverlap.AddDynamic(this, &APppChaseAICharacter::OnMeleeDamageOverlap);
    UE_LOG(LogTemp, Warning, TEXT("근접 공격 오버랩 이벤트가 바인딩되었습니다!")); // 디버깅용 로그

    // 임시 체력설정
    MaxHealth = 150.f;
    CurrentHealth = MaxHealth; // 또는 필요하다면 설정
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
    // 몽타주 노티파이에서 호출됨
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
        // 먼저 다른 AI와 충돌했는지 확인하고, 맞다면 함수를 즉시 종료합니다.
        /*APppChaseAICharacter* OtherAI = Cast<APppChaseAICharacter>(OtherActor);
        if (OtherAI)
        {
            UE_LOG(LogTemp, Warning, TEXT("AI가 다른 AI와 충돌했습니다. 공격을 무시합니다."));
            return; // 함수를 여기서 끝냅니다.
        }*/

        // 그 다음 플레이어 캐릭터인지 확인합니다.
        APppCharacter* PlayerCharacter = Cast<APppCharacter>(OtherActor);
        if (PlayerCharacter)
        {
            // 플레이어 캐릭터 찾기 성공!
            UE_LOG(LogTemp, Warning, TEXT("플레이어 캐릭터 찾기 성공!"));

            // 데미지 적용 로직은 이 블록 안에 있어야 합니다.
            MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            UGameplayStatics::ApplyDamage(
                PlayerCharacter,
                MeleeDamage,
                GetController(),
                this,
                UDamageType::StaticClass()
            );

            // 성공 로그도 이 블록 안에 있어야 합니다.
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
    GetWorldTimerManager().SetTimer(MeleeCollisionTimerHandle, this, &APppChaseAICharacter::ApplyMeleeDamage, 1.0f, false);
}

void APppChaseAICharacter::DeactivateMeleeDamageCollision()
{
    GetWorldTimerManager().ClearTimer(MeleeCollisionTimerHandle);
    MeleeDamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    UE_LOG(LogTemp, Warning, TEXT("MeleeDamageCollision 비활성화."));
}
