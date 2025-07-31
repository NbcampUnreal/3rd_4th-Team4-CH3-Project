#include "PPPGameMode.h"
#include "PPPGameState.h" // GameState캐스팅할 수 있게
#include "EngineUtils.h" //TActorIterator 사용
#include "DummyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameDefines.h"
#include "Engine/World.h"
#include "EnemySpawnVolume.h"



APPPGameMode::APPPGameMode()
{
    GameStateClass = APPPGameState::StaticClass(); // GameState를 우리가 만든 걸로 설정
    MaxRounds = 3;
    EnemiesPerRound = 5;
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

void APPPGameMode::StartRound() //라운드 시작시 적 초기화 수정
{
    // 1. 이전 라운드에서 살아남은 적 제거
    for (TActorIterator<ADummyEnemy> It(GetWorld()); It; ++It)
    {
        It->Destroy();
    }
    UE_LOG(LogEnemy, Warning, TEXT("이전 라운드 적 전부 제거됨"));

    // 2. 라운드 초기화
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (GS)
    {
        GS->SetGameState(EGameState::InProgress);
        // 라운드 증가 제거 → OnRoundCleared()에서만 증가
        GS->SetRemainingEnemies(EnemiesPerRound);
        UE_LOG(LogWave, Log, TEXT("Wave %d 시작!"), GS->CurrentRound);
    }

    // 3. 타이머 시작
    GS->StartRoundTimer(20.0f);

    // 4. 적 스폰
    SpawnEnemies();

    // 5. 보상 여부 초기화
    bRewardGiven = false;
}

void APPPGameMode::EndRound()
{
    SetGameState(EGameState::RoundEnded);

    APPPGameState* GS = GetGameState<APPPGameState>();
    if (!GS) return;

    UE_LOG(LogWave, Log, TEXT("라운드 %d 종료!"), GS->CurrentRound);

    // 점수 + 적 제거 조건 함께 판단
    bool bClearedByScore = GS->IsRoundCleared();
    bool bClearedByEnemies = (GS->RemainingEnemies <= 0);

    if (bClearedByScore || bClearedByEnemies)
    {
        UE_LOG(LogGame, Log, TEXT("조건 충족 → 라운드 클리어"));
        OnRoundCleared();
    }
    else
    {
        UE_LOG(LogGame, Warning, TEXT("조건 미달 → 게임 오버"));
        OnGameOver();
    }
}

void APPPGameMode::OnEnemyKilled()
{
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (!GS) return;

    int32 NewCount = GS->RemainingEnemies - 1;
    GS->SetRemainingEnemies(NewCount);

    UE_LOG(LogEnemy, Log, TEXT("적 처치! 남은 적: %d"), NewCount);

    CheckRewardCondition();

    // 모든 적을 처치한 경우
    if (NewCount <= 0)
    {
        UE_LOG(LogEnemy, Log, TEXT("모든 적 처치! 라운드 종료"));
        EndRound(); // 라운드 종료 처리
    }
    else
    {
        UE_LOG(LogEnemy, Log, TEXT("라운드 진행 중 - 남은 적: %d"), NewCount);
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

void APPPGameMode::CheckRewardCondition()
{
    // GameState에서 현재 점수 가져오기
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (!GS) return;

    // 이미 보상 지급했으면 중복 방지
    if (bRewardGiven) return;

    // 점수 조건 충족 확인
    if (GS->GetScore() >= 100)
    {
        bRewardGiven = true;

        // 보상 액터 클래스가 유효한지 확인
        if (RewardActorClass)
        {
            FVector SpawnLocation = FVector(0.0f, 0.0f, 1000.0f); // 하늘 위
            FRotator SpawnRotation = FRotator::ZeroRotator;
            FActorSpawnParameters SpawnParams;

            GetWorld()->SpawnActor<AActor>(RewardActorClass, SpawnLocation, SpawnRotation, SpawnParams);
            UE_LOG(LogGame, Warning, TEXT("점수 100 달성! 보상 액터가 떨어졌습니다."));
        }
        else
        {
            UE_LOG(LogGame, Warning, TEXT("RewardActorClass가 설정되지 않았습니다."));
        }

        // ✅ 점수 조건 만족 시 라운드 종료 조건 추가
        if (GS->GetCurrentState() == EGameState::InProgress)
        {
            UE_LOG(LogGame, Log, TEXT("점수 조건 충족 → 라운드 종료 호출"));
            EndRound();
        }
    }
}
// -------------------------------
// 클리어, 게임오버 구분 처리
// -------------------------------
void APPPGameMode::OnRoundCleared()
{
    UE_LOG(LogGame, Log, TEXT("라운드 클리어 - 다음 라운드 진행"));
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (GS)
    {
        //라운드 증가
        int32 NewRound = GS->CurrentRound + 1;
        GS ->SetCurrentRound(NewRound);
        //스코어 0
        GS->ResetScore(); // 스코어 초기화
    }
    StartRound(); // 다음 라운드 시작
}
void APPPGameMode::OnGameOver()
{
    UE_LOG(LogGame, Warning, TEXT("게임 오버"));
    //게임 오버 상태로
    //TODO : UI에 추가
}

