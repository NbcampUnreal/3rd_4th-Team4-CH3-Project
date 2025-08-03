#include "PickUpComponent.h"
#include "../Characters/PppCharacter.h"
#include "PickUpWeaponMaster.h"

UPickUpComponent::UPickUpComponent()
{
    SetGenerateOverlapEvents(true);
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    SetCollisionResponseToAllChannels(ECR_Ignore);
    SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UPickUpComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("PickUpComponent BeginPlay 실행됨"));

    OnComponentBeginOverlap.AddDynamic(this, &UPickUpComponent::OnBeginOverlap);
    OnComponentEndOverlap.AddDynamic(this, &UPickUpComponent::OnEndOverlap);
}

void UPickUpComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex,
                                      bool bFromSweep,
                                      const FHitResult& SweepResult)
{
    APppCharacter* PppCharacter = Cast<APppCharacter>(OtherActor);
    if (PppCharacter)
    {
        APickUpWeaponMaster* WeaponActor = Cast<APickUpWeaponMaster>(GetOwner());
        if (WeaponActor)
        {
            // 무기 이름 로그 출력
            if (WeaponActor->WeaponData.RowName != NAME_None)
            {
                UE_LOG(LogTemp, Warning, TEXT("오버랩 된 무기 이름: %s"), *WeaponActor->WeaponData.RowName.ToString());
            }

            // F 키 입력이 들어오면 장착 시도하도록 델리게이트 브로드캐스트
            WeaponPickUp.Broadcast(PppCharacter);
        }
    }
}

void UPickUpComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
                                    AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex)
{
    // 현재 구조에서는 특별히 해줄 게 없지만, 로그는 남겨둠
    APppCharacter* PppCharacter = Cast<APppCharacter>(OtherActor);
    if (PppCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("EndOverlap 발생 - 캐릭터가 무기 범위를 벗어남"));
    }
}
