// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldSpellManager.h"

AShieldSpellManager::AShieldSpellManager()
{

}

void AShieldSpellManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveSpells();
	CheckForCollisions();

}

void AShieldSpellManager::UpdateInstanceTransforms()
{

	if (!Caster) return;

	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			SpellInstances[i].Transform.SetLocation(Caster->GetActorLocation());
		}
	}
}
