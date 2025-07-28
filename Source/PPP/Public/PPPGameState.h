#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameDefines.h"
#include "PPPGameState.generated.h"

UCLASS()
class PPP_API APPPGameState : public AGameState
{
	GENERATED_BODY()

public:
	APPPGameState();

	/** 현재 게임 상태 (InProgress, GameOver 같은 거) */
	UPROPERTY(BlueprintReadOnly, Category="State")
	EGameState CurrentState;

	/** 현재 라운드 */
	UPROPERTY(BlueprintReadOnly, Category="State")
	int32 CurrentRound;

	/** 남은 적 */
	UPROPERTY(BlueprintReadOnly, Category="State")
	int32 RemainingEnemies;

	// -------------------------------
	// GameMode에서 호출할 Setter 함수들
	// -------------------------------

	void SetGameState(EGameState NewState);
	void SetCurrentRound(int32 Round);
	void SetRemainingEnemies(int32 Count);

	// -------------------------------
	// Getter 함수들
	// -------------------------------
	int32 GetCurrentRound() const;
	int32 GetRemainingEnemies() const; 

};
