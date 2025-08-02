#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "TestPlayerController.generated.h"

class UInputMappingContext; // IMC 관련 전방 선언
class UInputAction; // IA 관련 전방 선언
/**
 *
 */
UCLASS()
class PPP_API ATestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    ATestPlayerController();

    // 에디터에서 세팅할 IMC
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    UInputMappingContext* InputMappingContext;

    // IA_Move를 지정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    UInputAction* MoveAction;

    // IA_PickUp를 지정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
    UInputAction* PickUpAction;

    virtual void BeginPlay() override;

};
