#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameDebugUtility.generated.h"

/**
 * 게임 디버그 기능 전용 유틸리티 클래스
 * 레벨 재시작, 전체 적 제거 등 디버깅 목적 기능을 모아둠
 */
UCLASS(Blueprintable)
class PPP_API UGameDebugUtility : public UObject
{
	GENERATED_BODY()

public:
	// 디버그용 레벨 재시작
	UFUNCTION(BlueprintCallable, Category="Debug")
	static void RestartLevel(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="Debug")
	static void KillAllEnemies(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="Debug")
	static void EndWave(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="Debug")
	static void SkipToLastRound(UObject* WorldContextObject);
};
