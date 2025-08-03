#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TestEnemyKillQuest.h"
#include "TestQuestActorComponent.generated.h" // 이 줄은 파일 마지막에 있어야 함


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestProgressUpdated, int32, CurrentKills, int32, TargetKills);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStageCompleted, int32, CompletedStageIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllQuestsCompleted);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PPP_API UTestQuestActorComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UTestQuestActorComponent();

protected:

    virtual void BeginPlay() override;

public:

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 현재 활성화된 퀘스트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
    UTestEnemyKillQuest* CurrentQuest;

    // 퀘스트 목표 단계 배열 (20, 30, 40)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<int32> QuestStages;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
    int32 CurrentQuestStageIndex;


    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestProgressUpdated OnQuestProgressUpdated;


    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestStageCompleted OnQuestStageCompleted;


    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnAllQuestsCompleted OnAllQuestsCompleted;

    /**
     * @brief 적 처치 이벤트를 처리합니다. 이 함수를 적 AI가 죽을 때 호출해야 합니다.
     * @param KillAmount 처치한 적의 수 (기본값 1)
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void OnEnemyKilled(int32 KillAmount = 1);

    /**
     * @brief 퀘스트를 시작합니다.
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void StartQuest();

    /**
     * @brief 다음 퀘스트 단계로 진행합니다.
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void GoToNextQuestStage();

    /**
     * @brief 모든 퀘스트가 완료되었는지 확인합니다. UFUNCTION은 AI의 힘을 받음
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool AreAllQuestsCompleted() const;
};
