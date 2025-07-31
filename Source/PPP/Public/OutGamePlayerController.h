#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
    // by Team4 (yeoul)
    AOutGamePlayerController();

    // 메인 메뉴 표시
    // - MainMenu 위젯을 생성하고 화면에 표시
    // - bIsRestart가 true면 Start 버튼 텍스트 변경
    // by Team4 (yeoul)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMainMenu(bool bIsRestart);

    // 게임 시작
    // - 레벨 전환을 통해 인게임으로 이동
    // - 이후 GameInstance 초기화 로직 예정
    // by Team4 (yeoul)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void StartGame();

protected:
    // BeginPlay 오버라이드
    // - 현재 레벨이 MainMenuLevel이면 MainMenu UI 표시
    // by Team4 (yeoul)
    virtual void BeginPlay() override;

    // MainMenu 위젯 클래스
    // - BP에서 연결 (위젯 클래스 설정용)
    // by Team4 (yeoul)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

    // MainMenu 위젯 인스턴스
    // - C++에서 생성 및 표시
    // by Team4 (yeoul)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UUserWidget* MainMenuWidgetInstance;

    // TODO: 앞으로 추가될 PauseMenu, GameOver 위젯 인스턴스 포인터들 선언 예정
};
