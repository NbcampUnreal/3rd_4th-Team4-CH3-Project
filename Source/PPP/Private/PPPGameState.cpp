#include "PPPGameState.h"
#include "GameDefines.h"
#include "PPPGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

APPPGameState::APPPGameState()
{
	// 생성자에서는 아직 특별히 할 건 없음
	CurrentState = EGameState::WaitingToStart;
	CurrentRound = 0;
	RemainingEnemies = 0;

    PrimaryActorTick.bCanEverTick = true;// 타이머 작동을 위해 Tick 활성화
    RemainingTime = 0.0f; //타이머
    bIsTimerRunning = false;//작동중?
}

// -------------------------------
// GameMode → GameState 데이터 전달 함수들
// -------------------------------

void APPPGameState::SetGameState(EGameState NewState)
{
	CurrentState = NewState;
}

void APPPGameState::SetCurrentRound(int32 Round)
{
	CurrentRound = Round;
}

void APPPGameState::SetRemainingEnemies(int32 Count)
{
	RemainingEnemies = Count;
}

// -------------------------------
// Getter 함수들 정의
// -------------------------------
int32 APPPGameState::GetCurrentRound() const
{
	return CurrentRound;
}

int32 APPPGameState::GetRemainingEnemies() const
{
	return RemainingEnemies;
}

// -------------------------------
// Score
// -------------------------------
void APPPGameState::OnRep_Score()
{
    // UI에 연결 예정
}
void APPPGameState::AddScore(int32 Amount)
{
    Score += Amount;
    OnRep_Score(); // 점수 변경 시 UI 업데이트
    UE_LOG(LogGame, Log, TEXT("Score updated: %d"), Score);
}
void APPPGameState::ResetScore()
{
    Score = 0;
    OnRep_Score(); // 점수 초기화 시 UI 업데이트
    UE_LOG(LogGame, Log, TEXT("Score reset to 0"));
}
// -------------------------------
// Timer
// -------------------------------
void APPPGameState::StartRoundTimer(float Duration)
{
    RemainingTime = Duration;
    bIsTimerRunning = true;
}

void APPPGameState::StopRoundTimer()
{
    bIsTimerRunning = false;
}

void APPPGameState::OnRoundTimerFinished()
{
    bIsTimerRunning = false;

    // GameMode에서 라운드 종료 함수 호출
    APPPGameMode* GameMode = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        GameMode->EndRound(); // GameMode 쪽에 함수 있어야 작동함
    }
}

float APPPGameState::GetRemainingTime() const
{
    return RemainingTime;
}

void APPPGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsTimerRunning)
    {
        // 매 프레임마다 시간 감소
        RemainingTime -= DeltaTime;

        // 현재 남은 초 단위 시간 구함 (올림 처리)
        int32 CurrentSeconds = FMath::CeilToInt(RemainingTime);

        // 초 단위가 바뀌었을 때만 로그 출력 (중복 출력 방지)
        if (CurrentSeconds != PreviousDisplaySeconds)
        {
            PreviousDisplaySeconds = CurrentSeconds;
            UE_LOG(LogGame, Log, TEXT("Tick 작동 중 - 남은 시간(초): %d"), CurrentSeconds);
        }

        // 시간이 0 이하로 떨어졌을 경우 라운드 종료 처리
        if (RemainingTime <= 0.0f)
        {
            OnRoundTimerFinished();
        }
    }
}
