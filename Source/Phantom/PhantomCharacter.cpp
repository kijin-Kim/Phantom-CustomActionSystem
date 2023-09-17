// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhantomCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputBehavior.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Phantom.h"
#include "MotionWarpingComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"


APhantomCharacter::APhantomCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(RootComponent);

	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
}

void APhantomCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &APhantomCharacter::OnCombatSphereBeginOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &APhantomCharacter::OnCombatSphereEndOverlap);
	MaxWalkSpeedCache = GetCharacterMovement()->MaxWalkSpeed;
	MaxWalkSpeedCrouchedCache = GetCharacterMovement()->MaxWalkSpeedCrouched;
}

void APhantomCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AuthUpdateReplicatedAnimMontage(DeltaSeconds);
	if(IsLocallyControlled())
	{
		CalculateTargeted();	
	}
}

void APhantomCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CameraBoom->TargetOffset.Z += ScaledHalfHeightAdjust;
}

void APhantomCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CameraBoom->TargetOffset.Z -= ScaledHalfHeightAdjust;
}

void APhantomCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APhantomCharacter::Walk()
{
	if (!HasAuthority())
	{
		LocalWalk();
	}
	ServerWalk();
}

void APhantomCharacter::Run()
{
	if (!HasAuthority())
	{
		LocalRun();
	}
	ServerRun();
}

void APhantomCharacter::Sprint()
{
	if (!HasAuthority())
	{
		LocalSprint();
	}
	ServerSprint();
}

void APhantomCharacter::Dodge()
{
	if (!HasAuthority())
	{
		LocalDodge();
	}
	ServerDodge();
}

void APhantomCharacter::EnterStealthMode()
{
	if (IsSprinting())
	{
		Run();
	}
	Crouch();
}

void APhantomCharacter::LeaveStealthMode()
{
	UnCrouch();
}

void APhantomCharacter::Attack()
{
	if (!HasAuthority())
	{
		LocalAttack(CurrentTargetedEnemy);
	}
	ServerAttack(CurrentTargetedEnemy);
}

bool APhantomCharacter::CanCrouch() const
{
	return Super::CanCrouch() && !bIsDodging;
}

void APhantomCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

bool APhantomCharacter::CanDodge() const
{
	return !bIsDodging && !bIsCrouched;
}

bool APhantomCharacter::CanAttack() const
{
	return !bIsAttacking && !bIsCrouched && !bIsDodging;
}

bool APhantomCharacter::IsWalking() const
{
	if (GetVelocity().IsNearlyZero())
	{
		return false;
	}

	return !bIsCrouched
		       ? GetCharacterMovement()->MaxWalkSpeed >= MaxWalkSpeedCache && GetCharacterMovement()->MaxWalkSpeed <
		       MaxRunSpeed
		       : GetCharacterMovement()->MaxWalkSpeedCrouched >= MaxWalkSpeedCrouchedCache && GetCharacterMovement()->
		       MaxWalkSpeedCrouched < MaxRunSpeedCrouched;
}

bool APhantomCharacter::IsRunning() const
{
	if (GetVelocity().IsNearlyZero())
	{
		return false;
	}

	return !bIsCrouched
		       ? GetCharacterMovement()->MaxWalkSpeed >= MaxRunSpeed && GetCharacterMovement()->MaxWalkSpeed <
		       MaxSprintSpeed
		       : GetCharacterMovement()->MaxWalkSpeedCrouched >= MaxRunSpeedCrouched;
}

bool APhantomCharacter::IsSprinting() const
{
	if (GetVelocity().IsNearlyZero())
	{
		return false;
	}


	return !bIsCrouched ? GetCharacterMovement()->MaxWalkSpeed >= MaxSprintSpeed : false;
}

void APhantomCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APhantomCharacter, ReplicatedAnimMontage, COND_SimulatedOnly);
}

void APhantomCharacter::AuthUpdateReplicatedAnimMontage(float DeltaSeconds)
{
	if (HasAuthority() && GetMesh())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			ReplicatedAnimMontage.AnimMontage = AnimInstance->GetCurrentActiveMontage();
			ReplicatedAnimMontage.PlayRate = AnimInstance->Montage_GetPlayRate(ReplicatedAnimMontage.AnimMontage);
			ReplicatedAnimMontage.StartSectionName = AnimInstance->Montage_GetCurrentSection(ReplicatedAnimMontage.AnimMontage);
			ReplicatedAnimMontage.Position = AnimInstance->Montage_GetPosition(ReplicatedAnimMontage.AnimMontage);
			ReplicatedAnimMontage.bIsStopped = AnimInstance->Montage_GetIsStopped(ReplicatedAnimMontage.AnimMontage);
		}
	}
}

void APhantomCharacter::CalculateTargeted()
{
	if (EnemiesInCombatRange.IsEmpty())
	{
		CurrentTargetedEnemy = nullptr;
		return;
	}

	float MaxDot = 0.0f;
	AEnemy* NewTargetedCandidate = nullptr;
	const FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
	const FVector DotRight = LastInputVector.IsNearlyZero() ? FollowCamera->GetForwardVector() : LastInputVector; /* 플레이어의 입력이 있으면 입력을
	타겟팅에 반영합니다*/

	const TConsoleVariableData<bool>* CVar = IConsoleManager::Get().FindTConsoleVariableDataBool(TEXT("Phantom.Debug.Targeting"));
	const bool bDebugTargeting = CVar && CVar->GetValueOnGameThread();

	for (AEnemy* Enemy : EnemiesInCombatRange)
	{
		FVector PhantomToEnemy = (Enemy->GetActorLocation() - GetActorLocation());
		PhantomToEnemy.Normalize();

		const float DotResult = FVector::DotProduct(PhantomToEnemy, DotRight);
		if (MaxDot < DotResult)
		{
			MaxDot = DotResult;
			NewTargetedCandidate = Enemy;
		}

		if (bDebugTargeting)
		{
			if (UWorld* World = GetWorld())
			{
				DrawDebugString(World, FVector::ZeroVector, FString::Printf(TEXT("%f"), DotResult), Enemy, FColor::White, 0.0f, true);
			}
		}
	}

	if (NewTargetedCandidate)
	{
		if (!CurrentTargetedEnemy)
		{
			CurrentTargetedEnemy = NewTargetedCandidate;
		}
		else
		{
			FVector PhantomToTargeted = (CurrentTargetedEnemy->GetActorLocation() - GetActorLocation());
			PhantomToTargeted.Normalize();
			const float DotResult = FVector::DotProduct(PhantomToTargeted, DotRight);
			const float CURRENT_TARGETED_DOT_ADVANTAGE = 0.5f;
			if (DotResult < MaxDot - CURRENT_TARGETED_DOT_ADVANTAGE) // 현재 Targeted Enemy보다 더 정확하게 바라보고 있어야 새로운 타겟이 됨.
			{
				CurrentTargetedEnemy = NewTargetedCandidate;
			}
		}
	}

	if (bDebugTargeting)
	{
		if (UWorld* World = GetWorld())
		{
			if (NewTargetedCandidate && NewTargetedCandidate != CurrentTargetedEnemy)
			{
				DrawDebugSphere(World, NewTargetedCandidate->GetActorLocation(), 100.0f, 12, FColor::Blue, 0.0f, 0.0f);
			}

			if (CurrentTargetedEnemy)
			{
				DrawDebugSphere(World, CurrentTargetedEnemy->GetActorLocation(), 100.0f, 12, FColor::Red, 0.0f, 0.0f);
			}
		}
	}
}

void APhantomCharacter::OnCombatSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	AEnemy* NewEnemy = Cast<AEnemy>(OtherActor);
	if (NewEnemy)
	{
		EnemiesInCombatRange.AddUnique(NewEnemy);
		UE_LOG(LogPhantom, Warning, TEXT("CombatSphere BeginOverlap Actor Name: %s"), *OtherActor->GetName());
		UE_LOG(LogPhantom, Warning, TEXT("Total Enemies In Combat Range: %d"), EnemiesInCombatRange.Num());
	}
}

void APhantomCharacter::OnCombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                 int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	AEnemy* LeftEnemy = Cast<AEnemy>(OtherActor);
	if (LeftEnemy)
	{
		EnemiesInCombatRange.Remove(LeftEnemy);
		UE_LOG(LogPhantom, Warning, TEXT("CombatSphere EndOverlap Actor Name: %s"), *OtherActor->GetName());
		UE_LOG(LogPhantom, Warning, TEXT("Total Enemies In Combat Range: %d"), EnemiesInCombatRange.Num());
	}
}

void APhantomCharacter::LocalWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeedCache;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxWalkSpeedCrouchedCache;
}

void APhantomCharacter::ServerWalk_Implementation()
{
	LocalWalk();
}

void APhantomCharacter::LocalRun()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxRunSpeedCrouched;
}

void APhantomCharacter::ServerRun_Implementation()
{
	LocalRun();
}

void APhantomCharacter::LocalSprint()
{
	if (IsRunning()) // 현재 캐릭터가 Run하고 있을 때만 Sprint상태에 진입할 수 있다.
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
		if (bIsCrouched) // Crouch상태에서 Sprint상태에 진입하면 UnCrouch한다.
		{
			UnCrouch();
		}
	}
}

void APhantomCharacter::ServerSprint_Implementation()
{
	LocalSprint();
}

void APhantomCharacter::LocalDodge()
{
	if (CanDodge() && DodgeMontage)
	{
		bIsDodging = true;
		const float Duration = PlayAnimMontage(DodgeMontage);
		FTimerHandle DodgeEndTimer;
		GetWorldTimerManager().SetTimer(DodgeEndTimer, [this]()
		{
			bIsDodging = false;
		}, Duration, false);
	}
}

void APhantomCharacter::ServerDodge_Implementation()
{
	LocalDodge();
}

void APhantomCharacter::LocalAttack(AEnemy* AttackTarget)
{
	if (CanAttack() && AttackMontage)
	{
		if (AttackTarget)
		{
			MotionWarping->AddOrUpdateWarpTargetFromTransform(MotionWarpAttackTargetName, AttackTarget->GetActorTransform());
		}

		//bIsAttacking = true;
		const float Duration = PlayAnimMontage(AttackMontage);
		FTimerHandle AttackEndTimer;
		GetWorldTimerManager().SetTimer(AttackEndTimer, [this]()
		{
			bIsAttacking = false;
		}, Duration, false);
	}
}

void APhantomCharacter::ServerAttack_Implementation(AEnemy* AttackTarget)
{
	LocalAttack(AttackTarget);
}

void APhantomCharacter::OnRep_ReplicatedAnimMontage()
{
	// Simulated Proxy에서만 불림
	const TConsoleVariableData<bool>* CVar = IConsoleManager::Get().FindTConsoleVariableDataBool(TEXT("Phantom.Debug.AnimMontage"));
	bool bDebugRepAnimMontage = CVar && CVar->GetValueOnGameThread();
	if (bDebugRepAnimMontage)
	{
		if (ReplicatedAnimMontage.AnimMontage)
		{
			UE_LOG(LogPhantom, Warning, TEXT("Montage Name: %s"), *ReplicatedAnimMontage.AnimMontage->GetName());
		}
		UE_LOG(LogPhantom, Warning, TEXT("Play Rate: %f"), ReplicatedAnimMontage.PlayRate);
		UE_LOG(LogPhantom, Warning, TEXT("Position: %f"), ReplicatedAnimMontage.Position);
		UE_LOG(LogPhantom, Warning, TEXT("Start Section Name: %s"), *ReplicatedAnimMontage.StartSectionName.ToString());
		UE_LOG(LogPhantom, Warning, TEXT("bIsStopped %s"), ReplicatedAnimMontage.bIsStopped ? TEXT("True") : TEXT("False"));
	}


	const USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	if (!SkeletalMeshComponent)
	{
		return;
	}

	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	UAnimMontage* LocalActiveMontage = AnimInstance->GetCurrentActiveMontage();
	if (ReplicatedAnimMontage.AnimMontage && LocalActiveMontage != ReplicatedAnimMontage.AnimMontage)
	{
		AnimInstance->Montage_Play(ReplicatedAnimMontage.AnimMontage);
	}

	if (LocalActiveMontage)
	{
		if (ReplicatedAnimMontage.bIsStopped)
		{
			AnimInstance->Montage_Stop(LocalActiveMontage->BlendOut.GetBlendTime(), ReplicatedAnimMontage.AnimMontage);
			return;
		}

		if (AnimInstance->Montage_GetPlayRate(LocalActiveMontage) != ReplicatedAnimMontage.PlayRate)
		{
			AnimInstance->Montage_SetPlayRate(LocalActiveMontage, ReplicatedAnimMontage.PlayRate);
		}
		if (AnimInstance->Montage_GetCurrentSection(LocalActiveMontage) != ReplicatedAnimMontage.StartSectionName)
		{
			AnimInstance->Montage_JumpToSection(ReplicatedAnimMontage.StartSectionName);
		}

		const float MONTAGE_POSITION_DELTA_TOLERANCE = 0.1f;
		const float LocalMontagePosition = AnimInstance->Montage_GetPosition(LocalActiveMontage);
		if (!FMath::IsNearlyEqual(LocalMontagePosition, ReplicatedAnimMontage.Position, MONTAGE_POSITION_DELTA_TOLERANCE))
		{
			if (bDebugRepAnimMontage)
			{
				const float PositionDelta = FMath::Abs(LocalMontagePosition - ReplicatedAnimMontage.Position);
				UE_LOG(LogPhantom, Warning, TEXT("Adjusted Simulated Proxy Montage Position Delta. AnimNotify may be skipped. (Delta : %f)"),
				       PositionDelta);
			}
			AnimInstance->Montage_SetPosition(LocalActiveMontage, ReplicatedAnimMontage.Position);
		}
	}
}
