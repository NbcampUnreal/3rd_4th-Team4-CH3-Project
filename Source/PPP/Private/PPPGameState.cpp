#include "PPPGameState.h"
#include "GameDefines.h"
APPPGameState::APPPGameState()
{
	// 생성자에서는 아직 특별히 할 건 없음
	CurrentState = EGameState::WaitingToStart;
	CurrentRound = 0;
	RemainingEnemies = 0;
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