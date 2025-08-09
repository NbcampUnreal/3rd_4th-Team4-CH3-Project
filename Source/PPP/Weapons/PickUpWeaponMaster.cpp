#include "PickUpWeaponMaster.h"
#include "PickUpComponent.h"
#include "../Characters/PppCharacter.h"
#include "Engine/DataTable.h"
#include "../Weapons/EquipWeaponMaster.h"

APickUpWeaponMaster::APickUpWeaponMaster()
{
    PrimaryActorTick.bCanEverTick = false;

    // 상호작용을 위한 컴포넌트 생성 및 루트로 지정
    PickUpComp = CreateDefaultSubobject<UPickUpComponent>(TEXT("PickUpComp"));
    RootComponent = PickUpComp;

    // Static Mesh 생성 및 PickUpComp 하위(Attachment)로 설정
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(RootComponent);

    // 겹쳐진 캐릭터 초기화
    OverlappingCharacter = nullptr;
}

// 게임 시작 시 데이터테이블에서 무기 정보 로딩 및 이벤트 바인딩
void APickUpWeaponMaster::BeginPlay()
{
    Super::BeginPlay();

    // 무기 DataTable과 RowName이 유효하다면 데이터 로드
    if (WeaponData.DataTable && !WeaponData.RowName.IsNone())
    {
        const FWeaponRow* LoadedRow = WeaponData.DataTable->FindRow<FWeaponRow>(
            WeaponData.RowName, TEXT("Get WeaponRow from DataTable"));
        if (LoadedRow)
        {
            WeaponRow = *LoadedRow; // 반드시 WeaponRow 구조체 복사!, 무기 정보 구조체 복사
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("무기 데이터테이블에서 Row 찾기 실패!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponData DataTable/RowName이 None임!"));
    }

    // 픽업 컴포넌트의 무기 픽업 델리게이트에 HandlePickUp 함수 바인딩
    if (PickUpComp)
    {
        PickUpComp->WeaponPickUp.AddDynamic(this, &APickUpWeaponMaster::HandlePickUp);
    }
}

// 캐릭터가 무기 픽업 시 동작(기존 장비 해제, 신규 장비 장착 등)
void APickUpWeaponMaster::HandlePickUp(APppCharacter* Character)
{
    if (!Character) return;  // 캐릭터 존재하지 않으면 종료

    // 기존 장비가 있으면 월드에 드롭 및 장비 제거 (무기 교체 기능)
    if (Character->EquippedWeapon)
    {
        FWeaponRow PrevWeaponRow = Character->EquippedWeapon->GetWeaponDataRow();
        FVector DropLocation = Character->EquippedWeapon->GetActorLocation();
        FRotator DropRotation = Character->EquippedWeapon->GetActorRotation();

        Character->DropWeaponToWorld(PrevWeaponRow, DropLocation, DropRotation);

        Character->EquippedWeapon->Destroy();
        // Character->EquippedWeapon = nullptr; // 준모님 여기 빼도 돼요?
    }

    // 신규 장비가 존재하면 스폰 후 장착 및 OnEquipped 호출 (무기 장착 기능)
    if (WeaponRow.EquipWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("스폰 시도 - Skeletal Weapon: %s"), *WeaponRow.EquipWeapon->GetName());
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Character;

        AEquipWeaponMaster* NewWeapon = Character->GetWorld()->SpawnActor<AEquipWeaponMaster>(
            WeaponRow.EquipWeapon,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParams
        );
        if (NewWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("NewWeapon 스폰 성공! OnEquipped 호출"));
            NewWeapon->OnEquipped(Character, WeaponRow);
            // Character->EquippedWeapon = NewWeapon;   // 준모님 아랫줄의 세터로 바꿔도 돼요?
            // by Yeoul
            // 세터로 교체 (여기서 OnWeaponChanced 브로드캐스트됨)
            Character->SetEquippedWeapon(NewWeapon);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("NewWeapon SK 스폰 실패"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponRow.EquipWeapon이 None/Invalid"));
    }

    // 현재 PickUpActor(무기 오브젝트) 파괴 → 월드에 더 이상 존재하지 않음
    Destroy();

    // 플레이어의 겹쳐진 PickUpActor 관련 변수 초기화
    Character->OverlappingPickUpActor = nullptr;
}
