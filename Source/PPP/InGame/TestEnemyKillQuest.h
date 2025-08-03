#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TestEnemyKillQuest.generated.h"

UENUM(BlueprintType)
enum class EQuestState : uint8
{
    NotStarted UMETA(DisplayName = "Not Started"),
    InProgress UMETA(DisplayName = "In Progress"),
    Completed UMETA(DisplayName = "Completed")
};


UCLASS(Blueprintable, BlueprintType)
class PPP_API UTestEnemyKillQuest : public UObject
{
    GENERATED_BODY()

public: // <--- 'public:' 접근 지정자는 이 위치에 있어야 합니다.

    UTestEnemyKillQuest(); // 생성자

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 TargetKillCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 CurrentKillCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Quest")
    EQuestState QuestState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestDescription;

    /**
     * @brief 퀘스트 진행 상황을 업데이트합니다.
     * @param KillAmount 증가시킬 처치 수 (기본값 1)
     * @return 퀘스트가 완료되었는지 여부
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool AddKillCount(int32 KillAmount = 1);

    /**
     * @brief 퀘스트를 다음 단계로 진행시킵니다.
     * @param NewTargetKillCount 새로운 목표 처치 수
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void AdvanceQuest(int32 NewTargetKillCount);

    /**
     * @brief 퀘스트를 완료 상태로 설정합니다.
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void CompleteQuest();

    /**
     * @brief 퀘스트를 초기화합니다. 이것도 AI의 힘을 받음
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void ResetQuest();
};
