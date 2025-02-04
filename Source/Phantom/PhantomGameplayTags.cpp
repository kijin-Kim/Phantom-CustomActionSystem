// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomGameplayTags.h"
#include "GameplayTagsManager.h"

namespace PhantomGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Dodge, "HeroAction.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Attack, "HeroAction.Attack");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_AttackEventHandler, "HeroAction.AttackEventHandler");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Aim, "HeroAction.Aim");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Climb, "HeroAction.Climb");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Parry, "HeroAction.Parry");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Parried, "HeroAction.Parried");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Execute, "HeroAction.Execute");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Executed, "HeroAction.Executed");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Ambush, "HeroAction.Ambush");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Ambushed, "HeroAction.Ambushed");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_HitReact, "HeroAction.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Dead, "HeroAction.Dead");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_InstantDead, "HeroAction.InstantDead");

	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Run, "HeroAction.Run");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Sprint, "HeroAction.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_Stealth, "HeroAction.Stealth");

	UE_DEFINE_GAMEPLAY_TAG(HeroAction_AIWalk, "HeroAction.AIWalk");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_AIRun, "HeroAction.AIRun");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_AIAttack, "HeroAction.AIAttack");
	UE_DEFINE_GAMEPLAY_TAG(HeroAction_ParryEventHandler, "HeroAction.ParryEventHandler");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Run, "Event.HeroAction.Trigger.Run");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Attack, "Event.HeroAction.Trigger.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_AttackEventHandler, "Event.HeroAction.Trigger.AttackEventHandler");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_HitReact, "Event.HeroAction.Trigger.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Ambush, "Event.HeroAction.Trigger.Ambush");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Ambushed, "Event.HeroAction.Trigger.Ambushed");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Parry, "Event.HeroAction.Trigger.Parry");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Parried, "Event.HeroAction.Trigger.Parried");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Execute, "Event.HeroAction.Trigger.Execute");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Executed, "Event.HeroAction.Trigger.Executed");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Dodge, "Event.HeroAction.Dodge");
	

	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_AIWalk, "Event.HeroAction.Trigger.AIWalk");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_AIRun, "Event.HeroAction.Trigger.AIRun");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_AIAttack, "Event.HeroAction.Trigger.AIAttack");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_ParryEventHandler, "Event.HeroAction.Trigger.ParryEventHandler");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_Dead, "Event.HeroAction.Trigger.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Trigger_InstantDead, "Event.HeroAction.Trigger.InstantDead");
	

	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_CanTrigger_Ambush_Succeed, "Event.HeroAction.CanTrigger.Ambush.Succeed");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_CanTrigger_Ambush_Failed, "Event.HeroAction.CanTrigger.Ambush.Failed");

	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Parry_Opened, "Event.HeroAction.Parry.Opened");
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Parry_Closed, "Event.HeroAction.Parry.Closed");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_HeroAction_Execute, "Event.HeroAction.Execute");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Notify_Combo_Opened, "Event.Notify.Combo.Opened");
	UE_DEFINE_GAMEPLAY_TAG(Event_Notify_Combo_Closed, "Event.Notify.Combo.Closed");
	UE_DEFINE_GAMEPLAY_TAG(Event_Notify_Collision_Opened, "Event.Notify.Collision.Opened");
	UE_DEFINE_GAMEPLAY_TAG(Event_Notify_Collision_Closed, "Event.Notify.Collision.Closed");
	UE_DEFINE_GAMEPLAY_TAG(Event_Notify_Attack_Completed, "Event.Notify.Attack.Completed");
}
