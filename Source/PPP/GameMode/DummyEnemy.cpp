// DummyEnemy.cpp

#include "DummyEnemy.h"
#include "PPPGameMode.h"
#include "PPPGameState.h" //score
#include "Kismet/GameplayStatics.h"
// asdasdasdasdasdasdasdsadasd
ADummyEnemy::ADummyEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// 초기 체력 (초기화는 BeginPlay에서 처리)
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
}

void ADummyEnemy::BeginPlay()
{
	Super::BeginPlay();

	// 체력 초기화
	CurrentHealth = MaxHealth;
}

void ADummyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 데미지 처리 함수
float ADummyEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return 0.0f;
	}

	CurrentHealth -= DamageAmount;
	UE_LOG(LogEnemy, Log, TEXT("DummyEnemy 데미지 입음: %.1f, 남은 체력: %.1f"), DamageAmount, CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		Kill();
	}

	return DamageAmount;
}

// 죽는 함수, 지금은 테스용으로 K 눌렀을 때 액터가 Kill처리 되게 만들어놨음
void ADummyEnemy::Kill()
{
    UE_LOG(LogEnemy, Warning, TEXT("DummyEnemy 사망"));

    // GameMode에 알림
    APPPGameMode* GM = Cast<APPPGameMode>(UGameplayStatics::GetGameMode(this));

    // GameState에 점수 추가
    UWorld* World = GetWorld();
    APPPGameState* GS = World ? World->GetGameState<APPPGameState>() : nullptr;

    if (GS)
    {
        GS->AddScore(10);
        UE_LOG(LogEnemy, Warning, TEXT("score + 10. current score : %d"), GS->Score);
    }

    if (GM)
    {
        GM->OnEnemyKilled();

        // 점수 조건으로도 라운드 클리어 판단
        if (GS && GS->IsRoundCleared())
        {
            UE_LOG(LogGame, Warning, TEXT("점수 조건 충족 → 라운드 클리어"));
            GM->EndRound();
        }
    }

    Destroy();
}
