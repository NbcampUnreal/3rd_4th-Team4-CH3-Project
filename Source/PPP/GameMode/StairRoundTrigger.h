// StairRoundTrigger.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StairRoundTrigger.generated.h"

class UBoxComponent;

/**
 * 계단 트리거
 * - 라운드 클리어 후 플레이어가 계단에 들어오면 지정한 라운드로 StartRound()
 * - RoundIndexToStart: 시작할 라운드 번호(예: 2층 → 2, 3층 → 3)
 * - bOnlyWhenRoundEnded: 이전 라운드가 끝났을 때만 동작
 * - bConsumeOnce: 한 번만 사용하고 비활성화
 * - bEnabled: 외부에서 On/Off 가능
 */
UCLASS()
class PPP_API AStairRoundTrigger : public AActor
{
    GENERATED_BODY()

public:
    AStairRoundTrigger();

    // 외부에서 활성/비활성 제어
    UFUNCTION(BlueprintCallable, Category="Stair")
    void SetEnabled(bool bInEnabled) { bEnabled = bInEnabled; }

protected:
    UPROPERTY(VisibleAnywhere, Category="Stair")
    UBoxComponent* Box;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stair")
    int32 RoundIndexToStart = 2; // 2층이면 2, 3층이면 3

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stair")
    bool bOnlyWhenRoundEnded = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stair")
    bool bConsumeOnce = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stair")
    bool bEnabled = true;
    // ===== 최종층 옵션 =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FinalFloor")
    bool bIsFinalFloor = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FinalFloor", meta=(EditCondition="bIsFinalFloor"))
    bool bTravelOnFinalFloor = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FinalFloor", meta=(EditCondition="bIsFinalFloor && bTravelOnFinalFloor"))
    FName TargetLevelName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FinalFloor", meta=(EditCondition="bIsFinalFloor"))
    bool bCallRoundClearedBeforeTravel = true;
    UFUNCTION()
    void OnBoxBeginOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    virtual void BeginPlay() override;
};
