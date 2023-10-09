// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroActionJob.h"
#include "HeroActionJob_WaitHeroActionPredictionConfirmed.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeroActionConfirmedAccepted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeroActionConfirmedDeclined);

/**
 * 
 */
UCLASS()
class PHANTOM_API UHeroActionJob_WaitHeroActionPredictionConfirmed : public UHeroActionJob
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Action|Job",
		meta = (DisplayName = "Wait HeroAction Prediction Confirmed", HidePin = "HeroAction", DefaultToSelf = "HeroAction", BlueprintInternalUseOnly = "true"))
	static UHeroActionJob_WaitHeroActionPredictionConfirmed* CreateHeroActionJobWaitHeroActionConfirmed(UHeroAction* HeroAction);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

private:
	void BroadcastConfirmationDelegate(bool bIsAccepted);

public:
	UPROPERTY(BlueprintAssignable)
	FOnHeroActionConfirmedAccepted OnAccepted;
	UPROPERTY(BlueprintAssignable)
	FOnHeroActionConfirmedDeclined OnDeclined;

private:
	FDelegateHandle Handle;
};