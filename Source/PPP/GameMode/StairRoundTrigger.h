#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "StairRoundTrigger.generated.h"

UCLASS()
class PPP_API AStairRoundTrigger : public AActor
{
    GENERATED_BODY()

public:
    AStairRoundTrigger();

    UPROPERTY(VisibleAnywhere, Category="Trigger")
    UBoxComponent* Box;

    // 이 트리거가 시작시킬 라운드 인덱스(또는 층 번호). 1층=1, 2층=2, 3층=3
    UPROPERTY(EditAnywhere, Category="Round")
    int32 RoundIndex = 1;

    // GameMode가 InProgress일 때는 동작 금지, RoundEnded일 때만 허용
    // 상황에 따라 BP에서 강제로 켜고 끌 수 있게 플래그도 둔다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round")
    bool bAllowWhenRoundEndedOnly = true;

    // 라운드 트리거 사용 가능/불가 토글
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Round")
    bool bEnabled = true;

    UFUNCTION(BlueprintCallable, Category="Round")
    void SetEnabled(bool bInEnabled) { bEnabled = bInEnabled; }

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
