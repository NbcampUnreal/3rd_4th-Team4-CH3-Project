// PGameInstance.cpp
#include "PGameInstance.h"

void UPGameInstance::AddReward(TSubclassOf<AActor> InClass, int32 InQuantity)
{
    if (!InClass || InQuantity <= 0) return;

    // 같은 클래스면 합산(중복 최소화)
    for (FPendingReward& R : RewardsToGrant)
    {
        if (R.RewardClass == InClass)
        {
            R.Quantity += InQuantity;
            return;
        }
    }

    FPendingReward NewR;
    NewR.RewardClass = InClass;
    NewR.Quantity = InQuantity;
    RewardsToGrant.Add(NewR);
}

void UPGameInstance::ConsumeRewards(TArray<FPendingReward>& OutRewards)
{
    OutRewards = RewardsToGrant;
    RewardsToGrant.Empty();
}
