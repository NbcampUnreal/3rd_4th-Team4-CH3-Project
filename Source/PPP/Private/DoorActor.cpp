#include "DoorActor.h"

ADoorActor::ADoorActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // 회전 기준이 되는 SceneComponent 생성 및 설정
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);

    // 문 메시 생성 및 RootScene에 부착
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootScene);

    // 메시 피벗 위치를 왼쪽으로 보정 (경첩 기준 회전을 위함)
    DoorMesh->SetRelativeLocation(FVector(0.f, -50.f, 0.f)); // 문 폭이 Y=100일 경우 왼쪽 끝으로 이동
}

void ADoorActor::BeginPlay()
{
    Super::BeginPlay();

    // 시작 시 회전값 저장
    InitialRotation = GetActorRotation();
}

void ADoorActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsOpening)
    {
        FRotator CurrentRotation = GetActorRotation();
        FRotator TargetRotation = InitialRotation + OpenRotation;

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, OpenSpeed);
        SetActorRotation(NewRotation);

        // 목표 회전에 도달하면 더 이상 회전하지 않음
        if (CurrentRotation.Equals(TargetRotation, 1.0f))
        {
            bIsOpening = false;
        }
    }
}

void ADoorActor::OpenDoor()
{
    bIsOpening = true;
    UE_LOG(LogTemp, Warning, TEXT("OpenDoor() 호출"));
}
