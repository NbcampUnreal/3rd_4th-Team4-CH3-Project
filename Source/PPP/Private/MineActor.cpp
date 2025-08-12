#include "../Public/MineActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AMineActor::AMineActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    // 콜리전: Pawn만 오버랩
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);
    TriggerBox->InitBoxExtent(FVector(40.f, 40.f, 20.f));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMineActor::OnTriggerBegin);
}

void AMineActor::BeginPlay()
{
    Super::BeginPlay();
    if (DamageTypeClass == nullptr)
    {
        DamageTypeClass = UDamageType::StaticClass();
    }
}

void AMineActor::OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                const FHitResult& SweepResult)
{
    if (bTriggered && bOneShot) return;
    if (!OtherActor) return;

    // 플레이어만 타격: Character 체크(또는 APppCharacter로 캐스팅해도 됨)
    if (!OtherActor->IsA(ACharacter::StaticClass())) return;

    bTriggered = true;

    // 데미지 적용: Instigator/DamageCauser는 이 지뢰 자신
    UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetInstigatorController(), this, DamageTypeClass);

    if (bOneShot)
    {
        // 즉시 제거하거나 이펙트 후 제거
        if (LifeSpanAfterTrigger > 0.f)
        {
            SetLifeSpan(LifeSpanAfterTrigger);
        }
        else
        {
            Destroy();
        }
    }
}

void AMineActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UE_LOG(LogTemp, Log, TEXT("MineActor is ticking..."));
    //없어도댐 틱함수 테스트용
}
