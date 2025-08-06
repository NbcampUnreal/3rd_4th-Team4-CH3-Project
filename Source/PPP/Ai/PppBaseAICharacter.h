#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PppBaseAICharacter.generated.h"

class UBehaviorTree;

UCLASS()
class PPP_API APppBaseAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APppBaseAICharacter();

protected:
	virtual void BeginPlay() override;

public:
    // 이 AI 캐릭터가 사용할 비헤이비어 트리 애셋
    UPROPERTY(EditAnywhere, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    // AI의 시야 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SightRadius;


};
