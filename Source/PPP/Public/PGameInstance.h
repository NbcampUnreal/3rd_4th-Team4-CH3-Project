// PGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PGameInstance.generated.h"

/**
 * 스테이지 간 이월할 보상 정보를 담는 구조체
 */
USTRUCT(BlueprintType)
struct FPendingReward
{
    GENERATED_BODY()

    // 스폰할 액터 클래스(아이템/보상)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reward")
    TSubclassOf<AActor> RewardClass = nullptr;

    // 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reward")
    int32 Quantity = 1;
};

/**
 * 맵 전환 간 생존하는 보상 큐 저장용 GameInstance
 */
UCLASS()
class PPP_API UPGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // stage2에서 적립해두는 보상 큐
    UPROPERTY(BlueprintReadWrite, Category="Reward")
    TArray<FPendingReward> RewardsToGrant;

public:
    // 보상 추가(API)
    UFUNCTION(BlueprintCallable, Category="Reward")
    void AddReward(TSubclassOf<AActor> InClass, int32 InQuantity = 1);

    // 보상 소모(API): 현재 큐를 Out으로 내보내고 내부 큐 비움
    UFUNCTION(BlueprintCallable, Category="Reward")
    void ConsumeRewards(TArray<FPendingReward>& OutRewards);
};
