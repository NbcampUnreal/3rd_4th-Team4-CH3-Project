#include "BTS_SetIsInvestigatingFalse.h"
#include "BehaviorTree/BlackboardComponent.h" // UBlackboardComponent를 사용하기 위해 필요

UBTS_SetIsInvestigatingFalse::UBTS_SetIsInvestigatingFalse()
{
    NodeName = TEXT("Set IsInvestigating to False"); // 비헤이비어 트리에서 보일 이름

}

void UBTS_SetIsInvestigatingFalse::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    // 현재 비헤이비어 트리의 블랙보드 컴포넌트를 가져옴
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {

        BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
        UE_LOG(LogTemp, Warning, TEXT("BTS_SetIsInvestigatingFalse: IsInvestigating set to FALSE."));
    }
}
