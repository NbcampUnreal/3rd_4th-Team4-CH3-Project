#pragma once

#include "CoreMinimal.h"
#include "PppBaseAICharacter.h"
#include "PppChaseAICharacter.generated.h"

UCLASS()
class PPP_API APppChaseAICharacter : public APppBaseAICharacter
{
	GENERATED_BODY()
public:
    APppChaseAICharacter();

protected:
    virtual void BeginPlay() override;
};
