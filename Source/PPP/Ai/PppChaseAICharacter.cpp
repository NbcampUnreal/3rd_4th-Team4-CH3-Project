#include "PppChaseAICharacter.h"
#include "PppChaseAIController.h"

APppChaseAICharacter::APppChaseAICharacter()
{
    // 이 캐릭터는 ChaseAIController에 의해 조종됨
    AIControllerClass = APppChaseAIController::StaticClass();
    // 월드에 배치되거나 스폰될 때 자동 Possess
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 임시 체력설정
    MaxHealth = 150.f;
    CurrentHealth = MaxHealth; // 또는 필요하다면 설정
    Defense = 10.f;
}

void APppChaseAICharacter::BeginPlay()
{
    Super::BeginPlay();
}
