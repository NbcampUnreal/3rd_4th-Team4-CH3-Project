#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PPPAICharacter.generated.h"

UCLASS()
class PPP_API APPPAICharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APPPAICharacter();

    // 이동속도
    UPROPERTY(EditAnywhere, Category = "AI")
    float WalkSpeed = 300.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float RunSpeed = 600.0f;

    // 속도 변경 함수
    void SetMovementSpeed(float NewSpeed);
protected:
    virtual void BeginPlay() override;
};
