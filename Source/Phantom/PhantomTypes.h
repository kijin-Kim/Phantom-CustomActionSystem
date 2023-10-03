﻿#pragma once

UENUM(BlueprintType)
enum class ECharacterActionState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Dodge UMETA(DisplayName = "Dodge"),
	Attack UMETA(DisplayName = "Attack"),
	Aim UMETA(DisplayName = "Aim"),
	Climb UMETA(DisplayName = "Climb"),
	Parry UMETA(DisplayName = "Parry"),
	Execute UMETA(DisplayName = "Execute"),
	HitReact UMETA(DisplayName = "HitReact"),
	Dead UMETA(DisplayName = "Dead"),
	Max UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class ECharacterMovementState : uint8
{
	Stop UMETA(DisplayName = "Stop"),
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Sprinting UMETA(DisplayName = "Sprinting"),
	Max UMETA(DisplayName = "MAX")
};
