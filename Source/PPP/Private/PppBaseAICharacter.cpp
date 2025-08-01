#include "PppBaseAICharacter.h"


APppBaseAICharacter::APppBaseAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;


    AutoPossessAI = EAutoPossessAI::Disabled;

    SightRadius = 1500.0f; // 기본 시야 반경
}

void APppBaseAICharacter::BeginPlay()
{
	Super::BeginPlay();

}


