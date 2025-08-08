#include "PickUpComponent.h"
#include "../Characters/PppCharacter.h"
#include "../Weapons/PickUpWeaponMaster.h"

UPickUpComponent::UPickUpComponent()
{
    SetGenerateOverlapEvents(true);  // 오버랩 이벤트 생성 활성화
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 쿼리 전용(물리 X)
    SetCollisionObjectType(ECC_WorldDynamic);  // 동적 오브젝트 타입
    SetCollisionResponseToAllChannels(ECR_Ignore);  // 모든 채널 기본 무시
    SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // 플레이어와 오버랩 감지
}

void UPickUpComponent::BeginPlay()
{
    Super::BeginPlay();

    // 플레이어가 영역에 들어올 때, 나갈 때 이벤트 바인딩
    OnComponentBeginOverlap.AddDynamic(this, &UPickUpComponent::OnBeginOverlap);
    OnComponentEndOverlap.AddDynamic(this, &UPickUpComponent::OnEndOverlap);
}

// Sphere 영역에 캐릭터가 들어올 때 호출(오버랩 시작)
void UPickUpComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex,
                                      bool bFromSweep,
                                      const FHitResult& SweepResult)
{
    // 오버랩한 액터가 플레이어면, 해당 무기 액터에 캐릭터 정보 설정
    APppCharacter* PppCharacter = Cast<APppCharacter>(OtherActor);
    APickUpWeaponMaster* WeaponActor = Cast<APickUpWeaponMaster>(GetOwner());
    if (PppCharacter && WeaponActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("PickUpComponent: Overlap 감지됨!"));
        WeaponActor->OverlappingCharacter = PppCharacter;
        PppCharacter->OverlappingPickUpActor = WeaponActor;
    }
}

// Sphere 영역에서 캐릭터가 나갈 때 호출(오버랩 종료)
void UPickUpComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
                                    AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex)
{
    APppCharacter* PppCharacter = Cast<APppCharacter>(OtherActor);
    APickUpWeaponMaster* WeaponActor = Cast<APickUpWeaponMaster>(GetOwner());
    if (PppCharacter && WeaponActor)
    {
        // 나가는 플레이어가 해당 무기와 겹치고 있었으면 정보 초기화
        if (PppCharacter->OverlappingPickUpActor == WeaponActor)
            PppCharacter->OverlappingPickUpActor = nullptr;
    }
}

// 플레이어가 무기 픽업 명령(상호작용)을 실행할 때 호출, 이벤트로 전달
void UPickUpComponent::TryPickUp(APppCharacter* PickUpCharacter)
{
    WeaponPickUp.Broadcast(PickUpCharacter);
}
