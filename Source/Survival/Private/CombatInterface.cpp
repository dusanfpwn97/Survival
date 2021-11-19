// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatInterface.h"
#include "BaseSpell.h"
#include "BaseEnemy.h"
#include "BaseSpellManager.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.

void ICombatInterface::OnCollidedWithSpell_Implementation(ABaseSpellManager* Spell)
{

}

void ICombatInterface::SetTarget_Implementation(AActor* TargetActor)
{
	
}

FVector ICombatInterface::GetSpellCastLocation_Implementation()
{
	return FVector();
}

void ICombatInterface::SetSpellManager_Implementation(ABaseSpellManager* SpellManager)
{

}

TArray<AActor*> ICombatInterface::GetAliveEnemies()
{
	return TArray<AActor*>();
}

bool ICombatInterface::GetIsAlive()
{
	return false;
}

AActor* ICombatInterface::GetClosestEnemy()
{
	return nullptr;
}
