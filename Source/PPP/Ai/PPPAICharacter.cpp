#include "PPPAICharacter.h"
#include "PPPAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

APPPAICharacter::APPPAICharacter()
{
    AIControllerClass = APPPAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->MaxWalkSpeed = WalkSpeed;
    // 이동 방향으로 캐릭터 회전 항상 true로 할 것
    Movement->bOrientRotationToMovement = true;
    // AI가 방향을 바꿔서 이동할 때 몸을 돌리는 회전 값 (Yaw)
    Movement->RotationRate = FRotator(0.0f, 45.0f, 0.0f);
    // 점프 할 때 이동
    Movement->AirControl = 0.2f;
}

void APPPAICharacter::BeginPlay()
{
    Super::BeginPlay();


    UE_LOG(LogTemp, Warning, TEXT("[PPP] AI Character has been spawned"));

    // 컨트롤러가 정상적으로 붙었는지 확인하는 디버깅용 코드
    // AIController나 PlayerController가 할당되지 않았을 수 있기 때문
    // GetController()로 컨트롤러를 받아오고, nullptr 여부를 체크
    AController* MyController = GetController();
    if (MyController)
    {
        // 컨트롤러가 존재하면 이름을 출력
        UE_LOG(LogTemp, Warning, TEXT("[PPP] AI's controller is: %s"), *MyController->GetName());
    }
    else
    {
        // 컨트롤러가 없으면 nullptr 출력
        UE_LOG(LogTemp, Warning, TEXT("[PPP] AI's Controller is nullptr"));
    }
}

void APPPAICharacter::SetMovementSpeed(float NewSpeed)
{
    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->MaxWalkSpeed = NewSpeed;
        UE_LOG(LogTemp, Warning, TEXT("[Sparta] AI Speed changed: %.1f"), NewSpeed);
    }
}
