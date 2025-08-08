#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PickUpComponent.generated.h"

class APppCharacter;

// 무기 픽업 이벤트 델리게이트: 캐릭터를 파라미터로 넘겨줌
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponPickUp, APppCharacter*, PickUpCharacter);

// 무기 픽업 감지를 위한 Sphere Collision 컴포넌트. 플레이어가 영역에 진입 시 상호작용 트리거
UCLASS()
class PPP_API UPickUpComponent : public USphereComponent
{
    GENERATED_BODY()

public:
    UPickUpComponent();

    // 무기 픽업 델리게이트: Blueprint에서 바인딩 가능, TryPickUp 시 브로드캐스트됨
    UPROPERTY(BlueprintAssignable)
    FWeaponPickUp WeaponPickUp;

    // 무기 픽업 시도 함수: PickUpCharacter를 받아 WeaponPickUp 델리게이트 실행 (무기 획득 로직 트리거)
    UFUNCTION()
    void TryPickUp(APppCharacter* PickUpCharacter);

protected:
    virtual void BeginPlay() override;

    // 콜리전 오버랩 시작시 호출: 플레이어가 영역에 들어올 때 실행
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex,
                        bool bFromSweep,
                        const FHitResult& SweepResult);

    // 콜리전 오버랩 종료시 호출: 플레이어가 영역에서 나갈 때 실행
    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
                      AActor* OtherActor,
                      UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex);
};
