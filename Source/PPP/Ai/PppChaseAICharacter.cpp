#include "PppChaseAICharacter.h"
#include "PppChaseAIController.h"

APppChaseAICharacter::APppChaseAICharacter()
{
    // 이 캐릭터는 ChaseAIController에 의해 조종됨
    AIControllerClass = APppChaseAIController::StaticClass();
    // 월드에 배치되거나 스폰될 때 자동 Possess
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APppChaseAICharacter::BeginPlay()
{
    Super::BeginPlay();
}
