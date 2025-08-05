#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PickUpComponent.generated.h"

class APppCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponPickUp, APppCharacter*, PickUpCharacter);

UCLASS()
class PPP_API UPickUpComponent : public USphereComponent
{
    GENERATED_BODY()

public:
    UPickUpComponent();

    UPROPERTY(BlueprintAssignable)
    FWeaponPickUp WeaponPickUp;

    UFUNCTION()
    void TryPickUp(APppCharacter* PickUpCharacter);

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex,
                        bool bFromSweep,
                        const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
                      AActor* OtherActor,
                      UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex);
};
