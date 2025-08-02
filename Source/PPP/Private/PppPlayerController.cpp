// Copyright Epic Games, Inc. All Rights Reserved.

#include "PppPlayerController.h"
#include "PPPGameState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PppCharacter.h"

APppPlayerController::APppPlayerController() :
	InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	CrouchAction(nullptr),
    PovChangeAction(nullptr)
{
}

void APppPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
