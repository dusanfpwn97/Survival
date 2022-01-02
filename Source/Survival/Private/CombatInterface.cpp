// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatInterface.h"
#include "BaseEnemy.h"
#include "BaseSpellManager.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.

void ICombatInterface::OnCollidedWithSpell(ABaseSpellManager* Spell, SpellModifier SpellModifierThatDamaged)
{

}

void ICombatInterface::SetTarget(AActor* TargetActor)
{
	
}

FVector ICombatInterface::GetSpellCastLocation()
{
	return FVector();
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

TArray<AActor*> ICombatInterface::GetClosestEnemies(int32 NumOfEnemies)
{
	return TArray<AActor*>();
}

void ICombatInterface::OnHit(float Damage)
{

}

void ICombatInterface::OnDeath()
{

}
