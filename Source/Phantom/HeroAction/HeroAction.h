// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "HeroActionTypes.h"
#include "Phantom/ReplicatedObject.h"
#include "HeroAction.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnHeroActionEndSignature)

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PHANTOM_API UHeroAction : public UReplicatedObject
{
	GENERATED_BODY()

public:
	template <class T>
	static T* NewHeroAction(AActor* ReplicationOwner, const UClass* Class, const FHeroActionActorInfo& HeroActionActorInfo)
	{
		T* MyObj = NewObject<T>(ReplicationOwner, Class);
		MyObj->InitHeroAction(HeroActionActorInfo);
		return MyObj;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CanTriggerHeroAction() const;
	virtual void TriggerHeroAction();
	UFUNCTION(BlueprintCallable)
	virtual void EndHeroAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Hero Action", meta = (DisplayName = "Can Trigger Hero Action"))
	bool BP_CanTriggerHeroAction() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Hero Action", meta = (DisplayName = "Trigger Hero Action"))
	void BP_TriggerHeroAction();
	UFUNCTION(BlueprintImplementableEvent, Category = "Hero Action", meta = (DisplayName = "On End Hero Action"))
	void BP_OnEndHeroAction();
	
	UFUNCTION(BlueprintCallable, Category = "HeroAction")
	void DispatchHeroActionEvent(FGameplayTag EventTag, FHeroActionEventData EventData);
	void BindTryTriggerEvent();

	EHeroActionNetMethod GetHeroActionNetMethod() const { return HeroActionNetMethod; }
	const FHeroActionActorInfo& GetHeroActionActorInfo() const { return HeroActionActorInfo; }
	FGameplayTag GetLifeTag() const { return LifeTag; }

private:
	void InitHeroAction(const FHeroActionActorInfo& InHeroActionActorInfo);
	void HandleTagOnTrigger();
	void HandleTagOnEnd();

public:
	FOnHeroActionEndSignature OnHeroActionEnd;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Replication")
	EHeroActionNetMethod HeroActionNetMethod;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Triggering")
	EHeroActionRetriggeringMethod HeroActionRetriggeringMethod;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "ActorInfo")
	FHeroActionActorInfo HeroActionActorInfo;

	// 이 Tag를 가진 Event가 Dispatch될 시 Trigger를 시도합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTag|EventTags")
	FGameplayTagContainer TriggerEventTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTag|Condition")
	FGameplayTagContainer RequiredTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTag|Condition")
	FGameplayTagContainer BlockedTags;
	
	// Trigger-End사이에 부여되는 Tag. Remove시 EndAction이 불림.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTag|Triggered")
	FGameplayTag LifeTag;
	FDelegateHandle LifeTagRemovalDelegateHandle;
	// Trigger시 Remove하는 Tag.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTag|Triggered")
	FGameplayTagContainer ConsumeTags;

private:
	bool bIsTriggering = false;
};