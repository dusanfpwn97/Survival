// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatInterface.h"
#include "BaseSpell.h"
#include "BaseEnemy.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.

void ICombatInterface::OnCollidedWithSpell_Implementation(ABaseSpell* Spell)
{

}

void ICombatInterface::OnCollidedWithEnemy_Implementation(ABaseEnemy* Enemy)
{

}

void ICombatInterface::SetTarget_Implementation(AActor* TargetActor)
{

}

FVector ICombatInterface::GetSpellCastLocation_Implementation()
{
	return FVector();
}
/*
TArray<AActor*> ICombatInterface::GetSpawnedEnemies_Implementation()
{

}*/