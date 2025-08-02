#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "OutGamePlayerController.generated.h"

/**
 * OutGame UI (MainMenu, PauseMenu, GameOver) 전용 PlayerController
 * - UI 표시 및 전환 담당
 * - UI는 C++에서 생성 후 Viewport에 추가
 * by Team4 (yeoul)
 */
UCLASS()
class PPP_API AOutGamePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // 생성자
    // - 위젯 관련 포인터 초기화
    AOutGamePlayerController();

    // 메인 메뉴 표시
    // - MainMenu 위젯을 생성하고 화면에 표시
    // - bIsRestart가 true면 Start 버튼 텍스트 변경
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMainMenu(bool bIsRestart);

    // 일시 정지 메뉴
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowPauseMenu();

    // 게임 시작
    // TODO: GameInstance 초기화 로직 예정
    UFUNCTION(BlueprintCallable, Category = "UI")
    void StartGame();

    // 게임 종료
    UFUNCTION(BlueprintCallable, Category = "UI")
    void QuitGame();

    // 게임 오버
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowGameOver();


protected:
    // BeginPlay 오버라이드
    // - 현재 레벨이 MainMenuLevel이면 MainMenu UI 표시
    // - PauseMenu 입력 매핑 등록
    virtual void BeginPlay() override;

    // ESC 입력 바인딩
    virtual void SetupInputComponent() override;

    // MainMenu 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

    // MainMenu 위젯 인스턴스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UUserWidget* MainMenuWidgetInstance;

    // Quit 사운드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* QuitSound;

    // PauseMenu 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;

    // PauseMenu 위젯 인스턴스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UUserWidget* PauseMenuWidgetInstance;

    // 입력 매핑 컨텍스트 (ESC 키 처리용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UInputMappingContext* PauseMenuIMC;
    // ESC 키에 바인딩된 InputAction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UInputAction* PauseMenuAction;

    // GameOver 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    // GameOver 위젯 인스턴스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UUserWidget* GameOverWidgetInstance;

    // ESC 키 입력 처리 함수
    UFUNCTION()
    void HandlePauseKey();

    // TODO: GameOver 위젯 인스턴스 포인터들 선언 예정
};
