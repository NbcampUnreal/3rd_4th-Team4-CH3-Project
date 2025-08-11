#include "DoorActor.h"
#include "Components/StaticMeshComponent.h"

ADoorActor::ADoorActor()
{
    PrimaryActorTick.bCanEverTick = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootScene);

    OpenHeight = 200.f;
    OpenSpeed = 2.f;

    bIsOpening = false;
    bIsClosing = false;
}

void ADoorActor::BeginPlay()
{
    Super::BeginPlay();
    InitialLocation = GetActorLocation();
}

void ADoorActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsOpening)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector TargetLoc = InitialLocation + FVector(0.f, 0.f, OpenHeight);

        FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLoc, DeltaTime, OpenSpeed);
        SetActorLocation(NewLocation);

        if (CurrentLocation.Equals(TargetLoc, 1.0f))
        {
            bIsOpening = false;
        }
    }
    else if (bIsClosing)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector NewLocation = FMath::VInterpTo(CurrentLocation, InitialLocation, DeltaTime, OpenSpeed);
        SetActorLocation(NewLocation);

        if (CurrentLocation.Equals(InitialLocation, 1.0f))
        {
            bIsClosing = false;
        }
    }
}

void ADoorActor::OpenDoor()
{
    bIsOpening = true;
    bIsClosing = false;
    UE_LOG(LogTemp, Warning, TEXT("문이 위로 열립니다."));
}

void ADoorActor::CloseDoor()
{
    bIsClosing = true;
    bIsOpening = false;
    UE_LOG(LogTemp, Warning, TEXT("문이 닫힙니다."));
}
