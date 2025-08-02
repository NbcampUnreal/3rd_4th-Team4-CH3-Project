// Fill out your copyright notice in the Description page of Project Settings.


#include "AITestCharacter.h"

// Sets default values
AAITestCharacter::AAITestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAITestCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAITestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAITestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

