﻿#pragma once

UENUM(BlueprintType)
enum class ECharacterActionState : uint8
{
	ECT_Idle UMETA(DisplayName = "Idle"),
	ECT_Dodge UMETA(DisplayName = "Dodge"),
	ECT_Attack UMETA(DisplayName = "Attack"),
	ECT_Aim UMETA(DisplayName = "Aim"),
	ECT_Climb UMETA(DisplayName = "Climb"),
	ECT_Parry UMETA(DisplayName = "Parry"),
	ECT_Execute UMETA(DisplayName = "Execute"),
	ECT_HitReact UMETA(DisplayName = "HitReact"),
	ECT_Dead UMETA(DisplayName = "Dead"),
	ECT_MAX UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class ECharacterMovementState : uint8
{
	EMT_Stop UMETA(DisplayName = "Stop"),
	EMT_Walking UMETA(DisplayName = "Walking"),
	EMT_Running UMETA(DisplayName = "Running"),
	EMT_Sprinting UMETA(DisplayName = "Sprinting"),
	EMT_MAX UMETA(DisplayName = "MAX")
};
