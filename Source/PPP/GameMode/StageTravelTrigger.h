#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "StageTravelTrigger.generated.h"

UCLASS()
class PPP_API AStageTravelTrigger : public AActor
{
    GENERATED_BODY()

public:
    AStageTravelTrigger();

    UPROPERTY(VisibleAnywhere, Category="Trigger")
    UBoxComponent* Box;

    // 이동 대상 맵 이름 (기본: stage1)
    UPROPERTY(EditAnywhere, Category="Travel")
    FName TargetLevelName = TEXT("stage1");

    UPROPERTY(VisibleAnywhere)
    bool bTriggered = false; //라운드 중복

    // 조건 충족 시에만 동작
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Travel")
    bool bTravelEnabled = false;

    UFUNCTION(BlueprintCallable, Category="Travel")
    void SetTravelEnabled(bool bEnabled) { bTravelEnabled = bEnabled; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Travel")
    bool bCallRoundClearedOnTravel = true;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnBoxBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);
};
