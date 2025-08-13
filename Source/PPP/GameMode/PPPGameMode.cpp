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
#include "Blueprint/UserWidget.h"         // [추가] UUserWidget 사용 안전 헤더

APPPGameMode::APPPGameMode()
{
    // 우리가 만든 GameState 사용
    GameStateClass = APPPGameState::StaticClass();

    // 기본 라운드/적 수
    MaxRounds = 3;
    EnemiesPerRound = 5;

    // 기본 플레이어 컨트롤러
    PlayerControllerClass = APppPlayerController::StaticClass();

    // [탁] 기본 자동 시작 대상 레벨: stage1, stage2
    AutoStartLevels = { FName(TEXT("stage1")), FName(TEXT("stage2")) };
}

void APPPGameMode::BeginPlay()
{
    Super::BeginPlay();

    // ===== [추가] 레벨별 라운드 기본 시간/모드 설정 (Stage1=60초, Stage2=120초) =====

    const FString Lraw = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefixString=*/true);
    FString L = Lraw.ToLower();
    L.ReplaceInline(TEXT("_"), TEXT(""));   // stage_1 → stage1
    L.ReplaceInline(TEXT(" "), TEXT(""));   // "stage 1" → stage1

    bUseStageTimer = false; // 전투 타이머 고정

    if (L.Contains(TEXT("stage2")) || L.Contains(TEXT("stage02")))
    {
        ExitWindowSeconds = 120.f;
        UE_LOG(LogGame, Log, TEXT("[GM] Stage2 설정 적용: ExitWindowSeconds=%.1f, bUseStageTimer=%s"),
               ExitWindowSeconds, bUseStageTimer ? TEXT("true") : TEXT("false"));
    }
    else if (L.Contains(TEXT("stage1")) || L.Contains(TEXT("stage01")))
    {
        ExitWindowSeconds = 60.f;
        UE_LOG(LogGame, Log, TEXT("[GM] Stage1 설정 적용: ExitWindowSeconds=%.1f, bUseStageTimer=%s"),
               ExitWindowSeconds, bUseStageTimer ? TEXT("true") : TEXT("false"));
    }
    else
    {
        ExitWindowSeconds = 60.f; // 기본
        UE_LOG(LogGame, Log, TEXT("[GM] Stage 설정 적용(기본): ExitWindowSeconds=%.1f, bUseStageTimer=%s"),
               ExitWindowSeconds, bUseStageTimer ? TEXT("true") : TEXT("false"));
    }


    /* 자동 라운드/스테이지 타이머 시작 제거 (ExitWindow에서만 켜도록 할게요!!) - 김여울
     *if (APPPGameState* GS = GetGameState<APPPGameState>())
    {
        GS->StartRoundTimer(120.f); // 2분 타이머 시작
    }
    */

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

    // [추가] Stage2 맵이면 스테이지 타이머 사용 플래그 자동 활성화 (BP에서 직접 켜도 됨) 우선끄고 테스트
    // {
    //     const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefixString=*/true);
    //     if (LevelName.Contains(TEXT("Stage2")))
    //     {
    //         bUseStageTimer = true;
    //     }
    // }

    /* 비긴 플레이에서 스테이지 바로 시작하지 않게 주석처리할게요!!! - 김여울
    // [추가] 스테이지 타이머 시작 (예: 120초). GameState의 라운드 타이머를 재사용.
    if (bUseStageTimer)
    {
        if (APPPGameState* GS = GetGameState<APPPGameState>())
        {
            GS->StartRoundTimer(StageTimerSeconds);
            UE_LOG(LogGame, Log, TEXT("스테이지 타이머 시작: %.1f초"), StageTimerSeconds);
        }
    }
    */

    // 정현성
    // 시간을 화면에 띄우기
    if (TimeWidgetClass)
    {
        UUserWidget* TimeWidget = CreateWidget<UUserWidget>(GetWorld(), TimeWidgetClass);
        if (TimeWidget)
        {
            TimeWidget->AddToViewport();
        }
    }

    // 김여울
    // 출구 게이트 캐싱 (Tag: ExitGateTag), 기본 비표시/충돌 off
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsWithTag(this, ExitGateTag, Found);
        ExitGate = Found.Num() > 0 ? Found[0] : nullptr;

        if (ExitGate)
        {
            ExitGate->SetActorHiddenInGame(true);
            ExitGate->SetActorEnableCollision(false);
        }
        else
        {
            UE_LOG(LogGame, Warning, TEXT("ExitGate(Tag=%s) not found."), *ExitGateTag.ToString());
        }
    }

    // [추가] 최종 확정값 로그
    UE_LOG(LogGame, Log, TEXT("[GM] Final Timer Setup → ExitWindowSeconds=%.1f, OverrideByLevel=%s, UseStageTimer=%s, GameMode=%s"),
           ExitWindowSeconds,
           bOverrideExitSecondsByLevel ? TEXT("true") : TEXT("false"),
           bUseStageTimer ? TEXT("true") : TEXT("false"),
           *GetClass()->GetName());

    // [자동 시작] 현재 레벨명이 AutoStartLevels에 들어있으면 라운드 자동 시작
    {
        const FString CurLevel = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefixString=*/true);
        UE_LOG(LogGame, Log, TEXT("[GameMode] BeginPlay. CurrentLevel='%s'"), *CurLevel);

        for (const FName& AutoName : AutoStartLevels)
        {
            // 부분 일치 허용(예: "Stage2_P" 같은 변형 대응)
            if (CurLevel.Contains(AutoName.ToString(), ESearchCase::IgnoreCase, ESearchDir::FromStart))
            {
                UE_LOG(LogGame, Log, TEXT("[GameMode] Level '%s' 진입 → 자동 StartRound() 실행"), *CurLevel);
                StartRound();
                break;
            }
        }
    }
}



    // ===== [추가 끝] =====

    // (만약 네 프로젝트에 "레벨 진입 시 자동 StartRound()" 블록이 아래에 있다면 그건 그대로 둬도 됨)




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
        GS->SetGameState(EGameState::InProgress); // [복구] 전투 상태로 진입

        // 라운드 증가 로직은 OnRoundCleared()에서만 처리
        GS->SetRemainingEnemies(EnemiesPerRound);
        UE_LOG(LogWave, Log, TEXT("Wave %d 시작!"), GS->CurrentRound);

        // [로그] 라운드 타이머 분기 확인
        UE_LOG(LogTemp, Warning, TEXT("[GM] StartRound: bUseStageTimer=%s"),
            bUseStageTimer ? TEXT("true") : TEXT("false"));

        // [통일] 라운드 타이머는 ExitWindowSeconds로 '한 번만' 시작
        GS->StartRoundTimer(ExitWindowSeconds);
        UE_LOG(LogTemp, Warning, TEXT("[GM] GS->StartRoundTimer(%.1f) 호출됨 (ExitWindowSeconds)"), ExitWindowSeconds);

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

    // 김여울
    // 출구 제한시간 시작 + 게이트 오픈
    StartExitWindow();

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

// =========================
// 김여울 아래 계속 추가
// =========================

void APPPGameMode::StartExitWindow()
{
    bGateOpen = true;

    if (ExitGate)
    {
        ExitGate->SetActorHiddenInGame(false);
        ExitGate->SetActorEnableCollision(true);
    }

    if (APPPGameState* GS = GetGameState<APPPGameState>())
    {
        GS->StartRoundTimer(ExitWindowSeconds);   // 시간이 끝나면 GameState→EndRound()→HasTimedOut() 경로로 GameOver 자동
    }

    UE_LOG(LogGame, Log, TEXT("출구 제한시간 시작: %.1f초"), ExitWindowSeconds);
}

void APPPGameMode::EnterNextStage()
{
    if (!bGateOpen)
    {
        return; // 아직 게이트 안열림
    }

    // 타이머 정지
    if (APPPGameState* GS = GetGameState<APPPGameState>())
    {
        GS->StopRoundTimer();
    }

    // 게이트 닫기
    bGateOpen = false;
    if (ExitGate)
    {
        ExitGate->SetActorEnableCollision(false);
        ExitGate->SetActorHiddenInGame(true);
    }

    // 다음 라운드 시작(또는 맵 로드로 교체 가능)
    StartRound();
}

void APPPGameMode::OnExitTimeOver()
{
    if (!bGateOpen)
    {
        return; // 이미 통과했을 수 있음
    }

    UE_LOG(LogGame, Warning, TEXT("출구 제한시간 초과 → GameOver"));
    bGateOpen = false;

    if (ExitGate)
    {
        ExitGate->SetActorEnableCollision(false);
        ExitGate->SetActorHiddenInGame(true);
    }

    OnGameOver();
}
