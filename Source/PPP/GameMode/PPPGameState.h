#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameDefines.h"
#include "PPPGameState.generated.h"

UCLASS()
class PPP_API APPPGameState : public AGameState
{
	GENERATED_BODY()

public:
	APPPGameState();

	/** 현재 게임 상태 (InProgress, GameOver 같은 거) */
	UPROPERTY(BlueprintReadOnly, Category="State")
	EGameState CurrentState;

	/** 현재 라운드 */
	UPROPERTY(BlueprintReadOnly, Category="State")
	int32 CurrentRound;

	/** 남은 적 */
	UPROPERTY(BlueprintReadOnly, Category="State")
	int32 RemainingEnemies;
    /** 점수 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
    int32 Score = 0;
    // 점수로 라운드 클리어 판단
    UFUNCTION(BlueprintCallable)
    bool IsRoundCleared() const;

    UFUNCTION()
    void OnRep_Score();

    UFUNCTION(BlueprintCallable)

    void AddScore(int32 Amount); //증가
    void ResetScore(); //스코어 초기화
    void StartRoundTimer(float Duration); //타이머 시작
    void StopRoundTimer(); //타이머 중지

    UFUNCTION(BlueprintCallable, Category = "Timer") // 정현성 Get Remaining Time 블루프린트로 호출
    float GetRemainingTime() const; //남은 시간 가져오기

    virtual void Tick(float DeltaTime) override;//tick사용
    EGameState GetCurrentState()const;
	// -------------------------------
	// GameMode에서 호출할 Setter 함수들
	// -------------------------------

	void SetGameState(EGameState NewState);
	void SetCurrentRound(int32 Round);
	void SetRemainingEnemies(int32 Count);

	// -------------------------------
	// Getter 함수들
	// -------------------------------
	int32 GetCurrentRound() const;
	int32 GetRemainingEnemies() const;
    UFUNCTION(BlueprintCallable)
    int32 GetScore() const { return Score; } //reward 확인용
    UPROPERTY(EditAnywhere, Category="Score")
    int32 ScoreToClearRound = 100; //라운드 클리어 점수

    UFUNCTION(BlueprintPure, Category="Timer")
    bool HasTimedOut() const { return bTimedOut; }//스테이지 타임아웃 여부

private:
    UPROPERTY(VisibleAnywhere, Category="Timer")
    float RemainingTime; //남은 시간
    bool bIsTimerRunning; //타이머 작동중인지
    void OnRoundTimerFinished(); //라운드 제한 시간 끝났을 때
    int32 PreviousDisplaySeconds = -1; //화면 표시용 이전 초 값을 저장 ! -1부터 시작해서 무조건 첫 프레임에 갱신되게
    bool bTimedOut = false;//스테이지 타임아웃 여부


};
