#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"

UCLASS()
class PPP_API ADoorActor : public AActor   // 여기 YOURPROJECT_API를 PPP_API로 변경
{
    GENERATED_BODY()

public:
    ADoorActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category="Door")
    void OpenDoor();

    UFUNCTION(BlueprintCallable, Category="Door")
    void CloseDoor();

private:
    UPROPERTY(VisibleAnywhere, Category="Components")
    USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere, Category="Components")
    UStaticMeshComponent* DoorMesh;

    UPROPERTY(EditAnywhere, Category="Door")
    float OpenHeight;

    UPROPERTY(EditAnywhere, Category="Door")
    float OpenSpeed;

    FVector InitialLocation;

    bool bIsOpening;
    bool bIsClosing;
};
