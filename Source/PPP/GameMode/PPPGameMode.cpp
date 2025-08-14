// PPPGameMode.cpp
#include "PPPGameMode.h"
#include "PPPGameState.h"                 // GameState 캐스팅
#include "../GameMode/PPPGameState.h"
#include "EngineUtils.h"                  // TActorIterator
#include "DummyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameDefines.h"
#include "PPPGameInstance.h"
#include "PppPlayerController.h"
#include "Engine/World.h"
#include "EnemySpawnVolume.h"
#include "PPPCharacter.h"                 // 플레이어 캐릭터
#include "../characters/PppPlayerController.h" // 플레이어 컨트롤러
#include "Blueprint/UserWidget.h"         // [추가] UUserWidget 사용 안전 헤더
#include "TestQuestActorComponent.h" // 정현성 퀘스트 추가
#include "GameOverWidget.h"

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

    // 정현성
    // 퀘스트 컴포넌트 생성, 등록
    QuestComponent = CreateDefaultSubobject<UTestQuestActorComponent>(TEXT("QuestComponent"));

}

UTestQuestActorComponent* APPPGameMode::GetQuestComponent() const
{
    return QuestComponent;
}


// void APPPGameMode::BindDeathEventsForExistingEnemies()
// {
//     UWorld* World = GetWorld();
//     if (!World) return;
//
//     for (TActorIterator<APppBaseAICharacter> It(World); It; ++It)
//     {
//         APppBaseAICharacter* Enemy = *It;
//         if (!IsValid(Enemy)) continue;
//
//         // 중복 바인딩 방지
//         Enemy->OnDeath.AddUniqueDynamic(this, &APPPGameMode::OnEnemyKilledFromDelegate);
//     }
// }
void APPPGameMode::BindDeathEventsForExistingEnemies()
{

    for (TActorIterator<APppBaseAICharacter> It(GetWorld()); It; ++It)
    {
        APppBaseAICharacter* Enemy = *It;
        if (!Enemy) continue;

    }
}

void APPPGameMode::FlagRoundClearedWithoutStarting()
{
    ++CurrentRound;
    TotalScore = 0;
    OnRoundClearedSignal.Broadcast();

    // StartRound 호출 X
    // StairRoundTrigger에서만 StartRound 시작하게 유도
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

    if (QuestComponent)
    {
        QuestComponent->StartQuest();
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

        BindDeathEventsForExistingEnemies();// 맵에 미리 배치된 적들까지 모두 구독
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
        GS->SetGameState(EGameState::InProgress);
        GS->SetRemainingEnemies(EnemiesPerRound);
        UE_LOG(LogWave, Log, TEXT("Wave %d 시작!"), GS->CurrentRound);

        // [수정] 라운드 타이머 초기화
        float InitTime = bUseStageTimer ? StageTimerSeconds : RoundTimePerRound;
        GS->SetRemainingTime(InitTime);
        UE_LOG(LogTemp, Warning, TEXT("[GM] RemainingTime 초기화: %.1f초"), InitTime);

        // [기존] ExitWindowSeconds 타이머 시작
        GS->StartRoundTimer(ExitWindowSeconds);
        UE_LOG(LogTemp, Warning, TEXT("[GM] GS->StartRoundTimer(%.1f) 호출됨 (ExitWindowSeconds)"), ExitWindowSeconds);
    }

    // 4) 적 스폰
    SpawnEnemies();

    // 새로 스폰된 적들도 포함해서 전부 구독
    BindDeathEventsForExistingEnemies();

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
        // OnGameOver();
        HandleGameOver();
        return;
    }

    UE_LOG(LogWave, Log, TEXT("라운드 %d 종료!"), GS->CurrentRound);

    // 점수 또는 적 처치 조건 충족 여부 판단
    //const bool bClearedByScore   = GS->IsRoundCleared();
    const bool bClearedByEnemies = (GS->RemainingEnemies <= 0);

    if (bClearedByEnemies)
    {
        UE_LOG(LogGame, Log, TEXT("조건 충족 → 라운드 클리어"));
        OnRoundCleared();
    }
    else
    {
        UE_LOG(LogGame, Warning, TEXT("조건 미달 → 게임 오버"));
        // OnGameOver();
        HandleGameOver();
    }
}

void APPPGameMode::OnEnemyKilled()
{
    UE_LOG(LogTemp, Warning, TEXT(">>>>> OnEnemyKilled 호출됨"));

    // 중복 제거!
    APPPGameState* GS = GetGameState<APPPGameState>();
    if (!GS) return;

    if (GS->GetCurrentState() != EGameState::InProgress)
    {
        UE_LOG(LogTemp, Warning, TEXT("적 처치 무시: 현재 상태는 전투 중이 아님"));
        return;
    }

    // 점수, 킬 수 한 번만 추가
    GS->AddScore(ScorePerKill);
    GS->AddKill();

    // 퀘스트만 업데이트
    if (QuestComponent)
    {
        QuestComponent->OnEnemyKilled(1);
    }

    const int32 NewCount = FMath::Max(GS->RemainingEnemies - 1, 0);
    GS->SetRemainingEnemies(NewCount);

    UE_LOG(LogEnemy, Log, TEXT("적 처치! 남은 적: %d, 현재 점수: %d"), NewCount, GS->GetScore());

    CheckRewardCondition();

    const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
    if (LevelName.Contains(TEXT("Stage1")) || LevelName.Contains(TEXT("stage1")))
    {
        UE_LOG(LogEnemy, Log, TEXT("Stage1 - 처치만으로 라운드 종료 안 함."));
        return;
    }

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
            // 현재 라운드와 일치하는 층의 스폰 볼륨만 스폰
            if (SpawnVolume->RoundIndexToSpawn == CurrentRound)
            {
                // EnemyClass 강제 설정
                if (EnemyClass)
                {
                    SpawnVolume->EnemyClasses.Empty();
                    SpawnVolume->EnemyClasses.Add(EnemyClass);
                    SpawnVolume->SpawnWeights.Empty();
                    SpawnVolume->SpawnWeights.Add(1.0f);
                }

                SpawnVolume->SpawnEnemies(EnemiesPerRound);
                BindDeathEventsForExistingEnemies(); // ✅ 스폰된 적들에게도 델리게이트 연결


                UE_LOG(LogEnemy, Log, TEXT("ROUND %d - %s 에서 적 %d마리 스폰됨"),
                    CurrentRound, *SpawnVolume->GetName(), EnemiesPerRound);
            }
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
        // 게임방식 변경되어 삭제예정!
        // // 점수 조건을 만족하면 라운드 종료
        // if (GS->GetCurrentState() == EGameState::InProgress)
        // {
        //     UE_LOG(LogGame, Log, TEXT("점수 조건 충족 → 라운드 종료 호출"));
        //     EndRound();
        // }
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

void APPPGameMode::HandleGameOver()
{
    UE_LOG(LogGame, Warning, TEXT("게임 오버"));

    // 점수 가져오기
    int32 FinalScore = 0;
    if (APPPGameState* GS = GetGameState<APPPGameState>())
    {
        FinalScore = GS->GetScore();
    }

    // 점수 저장
    if (UPPPGameInstance* GI = GetGameInstance<UPPPGameInstance>())
    {
        GI->FinalScore = FinalScore;
    }

    // GameOver 위젯 생성
    if (GameOverWidgetClass)
    {
        UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
        if (GameOverWidget)
        {
            GameOverWidget->AddToViewport();

            // ✅ 위젯 캐스팅해서 점수 세팅
            if (UGameOverWidget* TypedWidget = Cast<UGameOverWidget>(GameOverWidget))
            {
                if (UPPPGameInstance* GI = Cast<UPPPGameInstance>(GetGameInstance()))
                {
                    TypedWidget->SetFinalScore(GI->FinalScore);
                    UE_LOG(LogTemp, Warning, TEXT("✅ GameOverWidget에 점수 전달 완료: %d"), GI->FinalScore);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("GameInstance 캐스팅 실패"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("GameOverWidget 캐스팅 실패"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("GameOverWidget 생성 실패"));
        }
    }

    // HUD 제거 + 커서 표시
    if (APppPlayerController* PC = Cast<APppPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->SetHudWidgetsVisible(false);
        PC->bShowMouseCursor = true;

        FInputModeUIOnly UIOnly;
        UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(UIOnly);
    }

    // 일시정지
    UGameplayStatics::SetGamePaused(GetWorld(), true);

    // GameOver 레벨 이동
    UGameplayStatics::OpenLevel(this, GameOverLevelName);
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
void APPPGameState::AddKill()
{
    ++KillCount;
    UE_LOG(LogTemp, Log, TEXT("KillCount: %d"), KillCount);
}

int32 APPPGameState::GetKillCount() const
{
    return KillCount;
}

void APPPGameMode::OnGameOver()
{
    // 최종 점수를 PPPGameInstance에 저장
    int32 FinalScore = 0;
    if (APPPGameState* GS = GetGameState<APPPGameState>())
    {
        FinalScore = GS->GetScore();
    }

    if (UPPPGameInstance* GI = GetGameInstance<UPPPGameInstance>())
    {
        GI->FinalScore = FinalScore;
    }

    // 모든 HUD 위젯 숨기기
    if (APppPlayerController* PC = Cast<APppPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->SetHudWidgetsVisible(false);
        // C++ 코드로 마우스 커서 활성화 및 입력 모드 변경
        PC->bShowMouseCursor = true;
        FInputModeUIOnly UIOnly;
        PC->SetInputMode(UIOnly);
    }

    // 게임 오버 레벨로 이동
    UGameplayStatics::OpenLevel(this, FName("LV_GameOver"));
}
