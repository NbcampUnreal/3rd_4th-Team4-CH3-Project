#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyEnemy.generated.h"

UCLASS()
class PPP_API ADummyEnemy : public AActor
{
	GENERATED_BODY()
    
public:
	ADummyEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 데미지 처리 함수 오버라이딩
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// 죽음 처리
	UFUNCTION(BlueprintCallable)
	void Kill();

private:
	UPROPERTY(EditAnywhere, Category="Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, Category="Stats")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
};
