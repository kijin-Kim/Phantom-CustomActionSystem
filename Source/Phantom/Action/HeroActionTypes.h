#pragma once
#include "HeroActionTypes.generated.h"

// This Types are mostly impressed by GAS

class UHeroAction;
class APlayerController;
class AActor;

UENUM(BlueprintType)
enum class EHeroActionNetMethod
{
	// 클라이언트에서만 실행
	LocalOnly UMETA(DisplayName = "Local Only"),
	// 클라이언트에서 예측해서 실행하고 서버에 의해 Verify. 실패시 Rollback
	LocalPredicted UMETA(DisplayName = "Local Predicted"),
	// 서버에서 먼저 실행되고, 실행이 가능할 시 Owning Client도 따라서 실행함.
	ServerOriginated UMETA(DisplayName = "Server Originated"),
	// 서버에서만 실행됨.
	ServerOnly UMETA(DisplayName = "Server Only"),
	Max UMETA(hidden)
};

USTRUCT(BlueprintType)
struct PHANTOM_API FHeroActionActorInfo
{
	GENERATED_BODY()

	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> Owner;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> SourceActor;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<APlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UAnimInstance* GetAnimInstance() const;
	bool IsSourceLocallyControlled() const;
	bool IsOwnerHasAuthority() const;
};



