// GameDefines.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameDefines.generated.h"

//게임 전체 상태
UENUM(BlueprintType)
enum class EGameState : uint8
{
	WaitingToStart UMETA(DisplayName = "WaitingToStart"),    // 라운드 시작 전
	InProgress     UMETA(DisplayName = "InProgress"),        // 라운드 진행 중
	RoundEnded     UMETA(DisplayName = "RoundEnded"),        // 라운드 종료
	GameOver       UMETA(DisplayName = "GameOver")           // 전체 게임 종료
};
//로그 카테고리 선언
//가능하면 LogTemp 말고 이거 애용해주세요
//더 필요하신 카테고리 있으면 DM주세요
DECLARE_LOG_CATEGORY_EXTERN(LogGame, Log, All);     // 게임 전반 흐름
DECLARE_LOG_CATEGORY_EXTERN(LogWave, Log, All);     // 웨이브 관련
DECLARE_LOG_CATEGORY_EXTERN(LogEnemy, Log, All);    // 적 관련
DECLARE_LOG_CATEGORY_EXTERN(LogItem, Log, All);     // 아이템 관련
DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);       // UI 관련
DECLARE_LOG_CATEGORY_EXTERN(LogDebug, Log, All);    // 테스트/디버그용
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);  // 캐릭터 관련

