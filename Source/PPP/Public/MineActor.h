// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MineActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PPP_API AMineActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMineActor();

protected:

	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category="Mine")
    UBoxComponent* TriggerBox;

    UPROPERTY(VisibleAnywhere, Category="Mine")
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, Category="Mine|Damage")
    float DamageAmount = 30.0f;

    UPROPERTY(EditAnywhere, Category="Mine|Damage")
    TSubclassOf<UDamageType> DamageTypeClass;

    UPROPERTY(EditAnywhere, Category="Mine|Behavior")
    bool bOneShot = true;

    UPROPERTY(EditAnywhere, Category="Mine|Behavior", meta=(EditCondition="bOneShot==true"))
    float LifeSpanAfterTrigger = 0.1f;

    bool bTriggered = false;

    UFUNCTION()
    void OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                        const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
