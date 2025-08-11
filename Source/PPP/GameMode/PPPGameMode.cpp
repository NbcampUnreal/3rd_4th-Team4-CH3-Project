// PPPGameMode.cpp
#include "PPPGameMode.h"
#include "PPPGameState.h"                 // GameState 캐스팅
#include "EngineUtils.h"                  // TActorIterator
#include "DummyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameDefines.h"
#include "Engine/World.h"
#include "EnemySpawnVolume.h"
#include "PPPCharacter.h"                 // 플레이어 캐릭터
#include "../characters/PppPlayerController.h" // 플레이어 컨트롤러

APPPGameMode::APPPGameMode()
{
    // 우리가 만든 GameState 사용
    GameStateClass = APPPGameState::StaticClass();

    // 기본 라운드/적 수
    MaxRounds = 3;
    EnemiesPerRound = 5;

    // 기본 플레이어 컨트롤러
    PlayerControllerClass = APppPlayerController::StaticClass();
}

void APPPGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (APPPGameState* GS = GetGameState<APPPGameState>())
    {
        GS->StartRoundTimer(120.f); // 2분 타이머 시작
    }

    // 플레이어 사망 시 GameMode -> OnGameOver()로 연결
    APppCharacter* PppCharacter = Cast<APppCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (IsValid(PppCharacter))
    {
        PppCharacter->OnCharacterDead.AddDynamic(this, &APPPGameMode::OnGameOver);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("APppCharacter not found in GameMode::BeginPlay"));
    }

    // [추가] Stage2 맵이면 스테이지 타이머 사용 플래그 자동 활성화 (BP에서 직접 켜도 됨)
    {
        const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefixString=*/true);
        if (LevelName.Contains(TEXT("Stage2")))
        {
            bUseStageTimer = true;
        }
    }

    // [추가] 스테이지 타이머 시작 (예: 120초). GameState의 라운드 타이머를 재사용.
    if (bUseStageTimer)
    {
        if (APPPGameState* GS = GetGameState<APPPGameState>())
        {
            GS->StartRoundTimer(StageTimerSeconds);
            UE_LOG(LogGame, Log, TEXT("스테이지 타이머 시작: %.1f초"), StageTimerSeconds);
        }
    }
}

void APPPGameMode::SetGameState(EGameState NewState)
{
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (GS)
    {
        if (GS->CurrentState == NewState)
        {
            return;
        }
        GS->SetGameState(NewState);
    }

    // 상태 변화 로그
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
    // 1) 이전 라운드에서 남아있는 적 제거
    for (TActorIterator<ADummyEnemy> It(GetWorld()); It; ++It)
    {
        It->Destroy();
    }
    UE_LOG(LogEnemy, Warning, TEXT("이전 라운드 적 전부 제거됨"));

    // 2) 라운드 초기화
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (GS)
    {
        GS->SetGameState(EGameState::InProgress);
        // 라운드 증가 로직은 OnRoundCleared()에서만 처리
        GS->SetRemainingEnemies(EnemiesPerRound);
        UE_LOG(LogWave, Log, TEXT("Wave %d 시작!"), GS->CurrentRound);

        // 3) 타이머 시작 (GS 널 체크 내부에서 안전하게 처리)
        // [변경] 스테이지 타이머 사용 중이면 라운드 타이머는 켜지지 않음
        if (!bUseStageTimer)
        {
            GS->StartRoundTimer(20.0f);
        }
    }

    // 4) 적 스폰
    SpawnEnemies();

    // 5) 보상 플래그 초기화
    bRewardGiven = false;
}

void APPPGameMode::EndRound()
{
    SetGameState(EGameState::RoundEnded);

    APPPGameState* GS = GetGameState<APPPGameState>();
    if (!GS) return;


    // [추가] 시간 초과는 무조건 게임 오버
    if (GS->HasTimedOut())
    {
        UE_LOG(LogGame, Warning, TEXT("시간 초과 → 게임 오버"));
        OnGameOver();
        return;
    }

    UE_LOG(LogWave, Log, TEXT("라운드 %d 종료!"), GS->CurrentRound);

    // 점수 또는 적 처치 조건 충족 여부 판단
    const bool bClearedByScore   = GS->IsRoundCleared();
    const bool bClearedByEnemies = (GS->RemainingEnemies <= 0);

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

    const int32 NewCount = GS->RemainingEnemies - 1;
    GS->SetRemainingEnemies(NewCount);

    UE_LOG(LogEnemy, Log, TEXT("적 처치! 남은 적: %d"), NewCount);

    CheckRewardCondition();

    if (NewCount <= 0)
    {
        UE_LOG(LogEnemy, Log, TEXT("모든 적 처치! 라운드 종료"));
        EndRound();
    }
    else
    {
        UE_LOG(LogEnemy, Log, TEXT("라운드 진행 중 - 남은 적: %d"), NewCount);
    }
}

void APPPGameMode::OnPlayerDeath()
{
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
        if (AEnemySpawnVolume* SpawnVolume = Cast<AEnemySpawnVolume>(Actor))
        {
            // 여기서 EnemyClass 강제 주입
            if (EnemyClass)
            {
                SpawnVolume->EnemyClasses.Empty();
                SpawnVolume->EnemyClasses.Add(EnemyClass); // Chase, Flee 등 중 원하는 클래스
                SpawnVolume->SpawnWeights.Empty();
                SpawnVolume->SpawnWeights.Add(1.0f); // 확률 100%
            }

            SpawnVolume->SpawnEnemies(EnemiesPerRound);
            UE_LOG(LogEnemy, Log, TEXT("%s에서 %d마리 적 스폰 (강제 EnemyClass: %s)"),
                   *SpawnVolume->GetName(),
                   EnemiesPerRound,
                   EnemyClass ? *EnemyClass->GetName() : TEXT("기본"));
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
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (!GS) return;

    if (bRewardGiven) return;

    if (GS->GetScore() >= 40)
    {
        bRewardGiven = true;

        if (RewardActorClass)
        {
            // 보상 스폰 위치 예시: 특정 좌표
            const FVector  SpawnLocation = FVector(-500.0f, 500.0f, 0.0f);
            const FRotator SpawnRotation = FRotator::ZeroRotator;
            FActorSpawnParameters SpawnParams;

            GetWorld()->SpawnActor<AActor>(RewardActorClass, SpawnLocation, SpawnRotation, SpawnParams);
            UE_LOG(LogGame, Warning, TEXT("점수 40 달성! 보상 액터 스폰"));
        }
        else
        {
            UE_LOG(LogGame, Warning, TEXT("RewardActor가 설정되지 않았습니다."));
        }

        // 점수 조건을 만족하면 라운드 종료
        if (GS->GetCurrentState() == EGameState::InProgress)
        {
            UE_LOG(LogGame, Log, TEXT("점수 조건 충족 → 라운드 종료 호출"));
            EndRound();
        }
    }
}

void APPPGameMode::OnRoundCleared()
{
    UE_LOG(LogGame, Log, TEXT("라운드 클리어 - 다음 단계 준비"));

    // 라운드 수 증가 및 점수 초기화
    if (APPPGameState* GS = GetGameState<APPPGameState>())
    {
        const int32 NewRound = GS->CurrentRound + 1;
        GS->SetCurrentRound(NewRound);
        GS->ResetScore();
    }

    // 기존: StartRound()를 즉시 호출
    // 변경: 자동 시작을 막고, 다음 층으로 이동 가능 신호만 브로드캐스트
    OnRoundClearedSignal.Broadcast();

    // 자동으로 올리고 싶다면 아래처럼 사용:
    // CurrentFloor = FMath::Clamp(CurrentFloor + 1, 1, MaxFloors);
    // StartRound();
}

void APPPGameMode::OnGameOver()
{
    UE_LOG(LogGame, Warning, TEXT("게임 오버"));

    if (APppPlayerController* PC = Cast<APppPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->ShowGameOver();
    }
}
