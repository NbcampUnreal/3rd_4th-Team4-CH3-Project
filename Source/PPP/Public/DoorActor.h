#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"

UCLASS()
class PPP_API ADoorActor : public AActor
{
    GENERATED_BODY()

public:
    ADoorActor();

    // 문 열기 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    void OpenDoor();

protected:
    // 루트: 회전 중심이 되는 Scene 컴포넌트
    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootScene;

    // 문 메시 (RootScene에 부착)
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* DoorMesh;

    // 열리는 회전 각도 (예: Yaw = 90이면 오른쪽으로 90도 회전)
    UPROPERTY(EditAnywhere, Category = "Door")
    FRotator OpenRotation = FRotator(0.f, -90.f, 0.f);

    // 열리는 속도 (보간 속도)
    UPROPERTY(EditAnywhere, Category = "Door")
    float OpenSpeed = 2.0f;

    // 문이 열리고 있는 상태 여부
    bool bIsOpening = false;

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

private:
    // 초기 회전값 저장용
    FRotator InitialRotation;
};
