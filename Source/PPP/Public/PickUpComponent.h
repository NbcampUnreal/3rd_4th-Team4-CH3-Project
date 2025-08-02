#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PickUpComponent.generated.h"

// PickUp 대상에게 전달할 델리게이트 정의 (무기를 줍는 주체 = AActor)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponPickUp, AActor*, PickUpActor);

UCLASS()
class PPP_API UPickUpComponent : public USphereComponent
{
    GENERATED_BODY()

public:
    UPickUpComponent();

    // 외부에서 바인딩할 수 있는 델리게이트 (무기 줍기용)
    UPROPERTY()
    FWeaponPickUp WeaponPickUp;

private:
    virtual void BeginPlay() override;

    // 오버랩 감지 함수
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex,
                        bool bFromSweep,
                        const FHitResult& SweepResult);

    // 오버랩 종료 감지 (이번 구조에서는 특별히 사용 X)
    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
                      AActor* OtherActor,
                      UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex);
};
