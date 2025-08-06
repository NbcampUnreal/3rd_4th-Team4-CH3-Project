#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UUserWidget* GameOverWidgetInstance;

    // ====== 사운드 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* QuitSound;

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
private:
    UFUNCTION()
    void HandlePauseKey();
};
