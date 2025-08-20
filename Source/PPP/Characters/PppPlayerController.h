#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestQuestActorComponent.h" // 정현성 퀘스트UI 추가
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"
#include "PppPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;
class USoundBase;

UCLASS()
class PPP_API APppPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    APppPlayerController();

    // ====== 인풋 관련 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* ZoomAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* SprintAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* CrouchAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* PovChangeAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* PickUpAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* FireAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* ReloadAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* EquipRifleAction;


    // ====== UI 인풋/매핑 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UInputMappingContext* PauseMenuIMC;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UInputAction* PauseMenuAction;

    // ====== UI 위젯 클래스 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    // ====== UI 위젯 인스턴스 ======
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UUserWidget* MainMenuWidgetInstance;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UUserWidget* PauseMenuWidgetInstance;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "UI")
    UUserWidget* GameOverWidgetInstance;

    // 정현성
    // 킬 카운트 위젯 추가
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> QuestUIWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "UI")
    UUserWidget* QuestUIWidgetInstance;

    UPROPERTY()
    class UTextBlock* QuestProgressTextBlock;

    // ====== 사운드 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* QuitSound;

    // 김여울
    // HUD 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI|HUD")
    TSubclassOf<UUserWidget> HealthClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|HUD")
    TSubclassOf<UUserWidget> CrosshairClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|HUD")
    TSubclassOf<UUserWidget> WeaponTrayClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|HUD")
    TSubclassOf<UUserWidget> HitmarkerClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|HUD")
    TSubclassOf<UUserWidget> QuestClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|HUD")
    TSubclassOf<UUserWidget> TimeClass;

    UPROPERTY()
    UUserWidget* HealthWidget;

    UPROPERTY()
    UUserWidget* CrosshairWidget;

    UPROPERTY()
    UUserWidget* WeaponTrayWidget;

    UPROPERTY()
    UUserWidget* HitmarkerWidget;

    UPROPERTY()
    UUserWidget* QuestWidget;

    UPROPERTY()
    UUserWidget* TimeWidget;

    UFUNCTION(BlueprintCallable, Category = "UI|HUD")
    void SetHudWidgetsVisible(bool bVisible);

    bool bPauseOpen = false; // 토글용

    // ====== 함수들 ======
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMainMenu(bool bIsRestart);
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowPauseMenu();
    UFUNCTION(BlueprintCallable, Category = "UI")
    void StartGame();
    UFUNCTION(BlueprintCallable, Category = "UI")
    void QuitGame();
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowGameOver();
    UFUNCTION()
    void OnCharacterDead();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stage")
    FName StageLevelName = TEXT("Stage2");


private:
    UFUNCTION()
    void HandlePauseKey();

    UFUNCTION()
    void OnQuestProgressUpdated(int32 CurrentKills, int32 TargetKills);

    virtual void OnPossess(APawn* InPawn) override;

    void BindDeathDelegateToPawn(APawn* InPawn);

    // 김여울
    void EnsureHudWidgets(); // HUD 위젯 생성 보장용 함수
};
