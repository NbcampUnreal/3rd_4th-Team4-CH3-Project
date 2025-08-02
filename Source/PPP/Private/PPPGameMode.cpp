#include "PPPGameMode.h"
#include "PPPGameState.h" // GameState캐스팅할 수 있게
#include "EngineUtils.h" //TActorIterator 사용
#include "DummyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameDefines.h"
#include "Engine/World.h"
#include "EnemySpawnVolume.h"
#include "TestPlayerController.h" // PlayerController 클래스를 사용




APPPGameMode::APPPGameMode()
{
    GameStateClass = APPPGameState::StaticClass(); // GameState를 우리가 만든 걸로 설정
    MaxRounds = 3;
    EnemiesPerRound = 5;

    PlayerControllerClass = ATestPlayerController::StaticClass();

}

void APPPGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogGame, Warning, TEXT("BeginPlay 호출됨"));

    // 초기 게임 상태 설정
    SetGameState(EGameState::WaitingToStart);

    // 테스트용 자동 라운드 시작 (추후 타이머 지연 가능)
    StartRound();
}

void APPPGameMode::SetGameState(EGameState NewState)
{
    // GameState에서 현재 상태를 가져옴
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (GS)
    {
        if (GS->CurrentState == NewState)
            return;

        GS->SetGameState(NewState);
    }

    // 로그 출력 등 상태 변화 알림
    switch (NewState)
    {
        case EGameState::WaitingToStart:
            UE_LOG(LogGame, Log, TEXT("게임 상태: 대기 중"));
            break;

        case EGameState::InProgress:
            UE_LOG(LogGame, Log, TEXT("게임 상태: 전투 시작"));
            break;

        case EGameState::RoundEnded:
            UE_LOG(LogGame, Log, TEXT("게임 상태: 라운드 종료"));
            break;

        case EGameState::GameOver:
            UE_LOG(LogGame, Warning, TEXT("게임 상태: 게임 오버"));
            break;
    }
}

void APPPGameMode::StartRound()
{
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (GS)
    {
        GS->SetGameState(EGameState::InProgress);
        GS->SetCurrentRound(GS->CurrentRound + 1); //  GameState에 라운드 증가
        GS->SetRemainingEnemies(EnemiesPerRound);  //  적 수 설정

        UE_LOG(LogWave, Log, TEXT("Wave %d 시작!"), GS->CurrentRound);
    }

    SpawnEnemies();
}

void APPPGameMode::EndRound()
{
    SetGameState(EGameState::RoundEnded);

    APPPGameState* GS = GetGameState<APPPGameState>();
    if (!GS) return;

    UE_LOG(LogWave, Log, TEXT("라운드 %d 종료!"), GS->CurrentRound);

    // 라운드 증가 (StartRound에서 다시 세팅됨)
    if (GS->CurrentRound >= MaxRounds)
    {
        // 모든 라운드 완료 → 게임 클리어
        SetGameState(EGameState::GameOver);
        UE_LOG(LogGame, Warning, TEXT("게임 클리어!"));
    }
    else
    {
        // 다음 라운드 시작
        StartRound();
    }
}

void APPPGameMode::OnEnemyKilled()
{
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (!GS) return;

    int32 NewCount = GS->RemainingEnemies - 1;
    GS->SetRemainingEnemies(NewCount);

    UE_LOG(LogEnemy, Log, TEXT("적 처치! 남은 적: %d"), NewCount);

    if (NewCount <= 0)
    {
        EndRound();
    }
}

void APPPGameMode::OnPlayerDeath()
{
    // 플레이어 사망 → 게임 오버 처리
    UE_LOG(LogGame, Error, TEXT("플레이어 사망"));
    SetGameState(EGameState::GameOver);
}

void APPPGameMode::SpawnEnemies()
{
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(World, AEnemySpawnVolume::StaticClass(), FoundVolumes);

    if (FoundVolumes.IsEmpty())
    {
        UE_LOG(LogEnemy, Warning, TEXT("EnemySpawnVolume이 존재하지 않습니다."));
        return;
    }

    for (AActor* Actor : FoundVolumes)
    {
        AEnemySpawnVolume* SpawnVolume = Cast<AEnemySpawnVolume>(Actor);
        if (SpawnVolume && SpawnVolume->EnemyClass)
        {
            SpawnVolume->SpawnEnemies(EnemiesPerRound);
            UE_LOG(LogEnemy, Log, TEXT("%s에서 %d마리 적 스폰"), *SpawnVolume->GetName(), EnemiesPerRound);
        }
        else
        {
            UE_LOG(LogEnemy, Warning, TEXT("EnemyClass가 지정되지 않은 EnemySpawnVolume이 있음: %s"), *Actor->GetName());
        }
    }
}

int32 APPPGameMode::GetMaxRounds() const
{
    return MaxRounds;
}
