// by Yeoul
// FireMode Enum 공용 헤더

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
    Single UMETA(DisplayName="Single"),
    Auto   UMETA(DisplayName="Auto")
};
