#include "GameDebugUtility.h"
#include "DummyEnemy.h" // ← 이거 반드시 추가
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "PPPGameMode.h"

void UGameDebugUtility::RestartLevel(UObject* WorldContextObject)
{
	if (!WorldContextObject) return;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;

	FName CurrentLevelName = *World->GetName();
	UGameplayStatics::OpenLevel(WorldContextObject, CurrentLevelName, false);

	UE_LOG(LogTemp, Warning, TEXT("[GameDebugUtility] 레벨 재시작 실행됨: %s"), *CurrentLevelName.ToString());
}
void UGameDebugUtility::KillAllEnemies(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!World) return;

	for (TActorIterator<ADummyEnemy> It(World); It; ++It)
	{
		It->Destroy();
	}

	UE_LOG(LogTemp, Warning, TEXT("모든 적 제거 완료"));
}
void UGameDebugUtility::EndWave(UObject* WorldContextObject)
{
	UE_LOG(LogDebug, Warning, TEXT("EndWave() 실행됨"));

	APPPGameMode* GameMode = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode)
	{
		GameMode->EndRound(); // GameMode 내의 실제 로직 호출
	}
}

void UGameDebugUtility::SkipToLastRound(UObject* WorldContextObject)
{
	UE_LOG(LogDebug, Warning, TEXT("SkipToLastRound() 실행됨"));

	APPPGameMode* GameMode = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	APPPGameState* GameState = Cast<APPPGameState>(UGameplayStatics::GetGameState(WorldContextObject));

	if (GameMode && GameState)
	{
		GameState->SetCurrentRound(GameMode->GetMaxRounds());
		GameMode->StartRound();
	}
}