#include "PPPGameState.h"
#include "GameDefines.h"
#include "PPPGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"



APPPGameState::APPPGameState()
{
	CurrentState = EGameState::WaitingToStart;
	CurrentRound = 1;
	RemainingEnemies = 0;
    PrimaryActorTick.bCanEverTick = true;// 타이머 작동을 위해 Tick 활성화
    RemainingTime = 0.0f; //타이머
    bIsTimerRunning = false;//작동중?
    bTimedOut = false; //스테이지 타임아웃 여부
}
EGameState APPPGameState::GetCurrentState() const
{
    return CurrentState;
}
// -------------------------------
// GameMode → GameState 데이터 전달 함수들
// -------------------------------
void APPPGameState::SetRemainingTime(float NewTime)
{
    RemainingTime = NewTime;
}

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
void APPPGameState::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("[GS] BeginPlay: %s  CanEverTick=%s"),
        *GetClass()->GetName(),
        PrimaryActorTick.bCanEverTick ? TEXT("true") : TEXT("false"));

    // 혹시 비활성일 수 있어 강제 Enable
    SetActorTickEnabled(true);
    UE_LOG(LogTemp, Warning, TEXT("[GS] TickEnabled=%s"), IsActorTickEnabled() ? TEXT("true") : TEXT("false"));
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

bool APPPGameState::IsRoundCleared() const
{
    return Score >= ScoreToClearRound; // 목표 점수 이상인지 확인
}
// -------------------------------
// Timer
// -------------------------------
void APPPGameState::StartRoundTimer(float Duration)
{
    // RemainingTime = FMath::Max(0.0f, Duration); //음수 방지
    // RemainingTime = Duration;
    // bIsTimerRunning = true;
    // bTimedOut = false; //시작시 리셋
    // PreviousDisplaySeconds = -1;  //표시될 초 리셋
    // UE_LOG(LogTemp, Warning, TEXT("[GS] StartRoundTimer: Duration=%.2f"), RemainingTime);
    if (bIsTimerRunning)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GS] 타이머 이미 실행 중 → StartRoundTimer 중단"));
        return;
    }

    RemainingTime = Duration;
    bIsTimerRunning = true;
    bTimedOut = false;

    UE_LOG(LogTemp, Warning, TEXT("[GS] 라운드 타이머 시작: %.1f초"), Duration);

}

void APPPGameState::StopRoundTimer()
{
    bIsTimerRunning = false;
    UE_LOG(LogTemp, Warning, TEXT("[GS] StopRoundTimer: Remain=%.2f"), RemainingTime);

}

void APPPGameState::OnRoundTimerFinished()
{
    UE_LOG(LogTemp, Warning, TEXT("[GS] TimerFinished 호출"));
    UE_LOG(LogGame, Warning, TEXT("라운드 제한 시간 종료됨"));

    bIsTimerRunning = false; //타이머 중단
    bTimedOut = true; //스테이지 타임아웃 세팅

    // GameMode에서 라운드 종료 함수 호출
    APPPGameMode* GameMode = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        GameMode->EndRound(); // GameMode 쪽에 함수 있어야 작동함
    }

}

float APPPGameState::GetRemainingTime() const
{
    return FMath::Max(0.0f, RemainingTime);
}

void APPPGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsTimerRunning || bTimedOut) return;

    RemainingTime -= DeltaTime;

    if (RemainingTime <= 0.f)
    {
        RemainingTime = 0.f;
        bTimedOut = true;
        bIsTimerRunning = false; // 타이머 종료 처리

        UE_LOG(LogTemp, Warning, TEXT("[GS] 라운드 제한 시간 종료됨"));

        OnRoundTimerFinished();
        // 타이머 종료 후 처리
    }
        // 화면에 표시  정현성 UI 충돌 때문에 주석 처리
        //if (GEngine)
        //{
        //  GEngine->AddOnScreenDebugMessage(
        //        123456, // 고정 키(같은 메시지 갱신)
        //        1.0f,   // 1초 표시
        //        FColor::Yellow,
        //        FString::Printf(TEXT("TIME LEFT: %d s"), CurrentSeconds)
        //    );
        //}
        // 시간이 0 이하로 떨어졌을 경우 라운드 종료 처리

    }

