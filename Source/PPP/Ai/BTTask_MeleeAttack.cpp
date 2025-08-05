#include "BTTask_MeleeAttack.h"
#include "AIController.h"
#include "PppChaseAICharacter.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

// 생성자
UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
    // TickTask 함수를 사용하겠다고 엔진에 알림
    bNotifyTick = true;

    // 이 노드가 블랙보드 키를 사용하도록 설정
    // MeleeAttack은 블랙보드 키를 직접 사용하지 않으므로 필요에 따라 제거 가능
    NodeName = "Melee Attack";
}

// 태스크의 메모리 크기를 반환
uint16 UBTTask_MeleeAttack::GetInstanceMemorySize() const
{
    return sizeof(FBTMeleeAttackTaskMemory);
}

// 태스크 실행 시 호출됨
EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // 메모리 구조체 가져오기
    FBTMeleeAttackTaskMemory* MyMemory = (FBTMeleeAttackTaskMemory*)NodeMemory;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APppChaseAICharacter* AICharacter = Cast<APppChaseAICharacter>(AIController->GetPawn());
    if (!AICharacter)
    {
        return EBTNodeResult::Failed;
    }

    // AI 캐릭터의 근접 공격 함수를 호출하여 공격 애니메이션을 재생함
    AICharacter->StartMeleeAttack();

    // 공격 시작 로그 추가
    UE_LOG(LogTemp, Warning, TEXT("AI가 근접 공격을 시작했습니다!"));

    // 공격이 시작되었음을 메모리에 표시함
    MyMemory->bIsAttacking = true;

    // 태스크를 InProgress 상태로 유지하고, TickTask를 호출하도록 함
    return EBTNodeResult::InProgress;
}

// 태스크가 InProgress 상태일 때 매 프레임 호출됨
void UBTTask_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    // 메모리 구조체 가져오기
    FBTMeleeAttackTaskMemory* MyMemory = (FBTMeleeAttackTaskMemory*)NodeMemory;

    APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
    APppChaseAICharacter* AICharacter = Cast<APppChaseAICharacter>(AIPawn);

    // AI 캐릭터가 유효하고, 메모리에 공격 중인 상태일 때만 아래 로직을 실행함
    if (AICharacter && MyMemory->bIsAttacking)
    {
        UAnimInstance* AnimInstance = AICharacter->GetMesh()->GetAnimInstance();

        // 공격 몽타주가 유효하고, 현재 재생 중이 아닌지 확인
        if (AnimInstance && AICharacter->MeleeAttackMontage && !AnimInstance->Montage_IsPlaying(AICharacter->MeleeAttackMontage))
        {
            // 공격 애니메이션이 끝났으니, 공격 중이 아니라고 메모리에 표시함
            MyMemory->bIsAttacking = false;

            // 태스크를 성공 상태로 종료하고, 비헤이비어 트리를 업데이트
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}
