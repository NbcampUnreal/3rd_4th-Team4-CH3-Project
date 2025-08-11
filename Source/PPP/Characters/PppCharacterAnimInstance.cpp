// Copyright Epic Games, Inc. All Rights Reserved.

#include "PppCharacterAnimInstance.h"
#include "PppCharacter.h"
#include "KismetAnimationLibrary.h"

void UPppCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
}

void UPppCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 정현수 폰 오브젝트 값 가져오기
    APawn* Pawn = TryGetPawnOwner();
    if (!Pawn) return;

    // 정현수 캐릭터 오브젝트 값 가져오기
    APppCharacter* MyChar = Cast<APppCharacter>(Pawn);
    if (MyChar)
    {
        // 정현수 캐릭터 속도 값 가져오기
        FVector velocity = MyChar->GetVelocity();
        // 정현수 캐릭터의 위 아래 값은 반영 안 되게 막기
        velocity.Z = 0.0f;
        // 정현수 캐릭터의 이동 값 크기만큼 반환
        GroundSpeed = velocity.Size();

        // 정현수 애니메이션 라이브러리에서 이동 방향 값 반환해서 가져오기
        MoveDirection = UKismetAnimationLibrary::CalculateDirection(
            velocity, MyChar->GetActorRotation());
        // 정현수 방향 값 최대 최소 설정
        MoveDirection = FMath::Clamp(MoveDirection, -180.0f, 180.0f);

        // 캐릭터의 bool 값을 읽어와 동기화
        bIsRifleEquipped = MyChar->GetIsRifleEquipped();
    }
}
