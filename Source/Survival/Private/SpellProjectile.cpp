// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellProjectile.h"
#include "BaseSpellManager.h"

ASpellProjectile::ASpellProjectile()
{

}

void ASpellProjectile::Move()
{

	UWorld* World = GetWorld();
	if (!World || !SpellManager) return;

	FHitResult Hit;
	CurrentDirection = GetMoveDirection();
	AddActorWorldOffset(CurrentDirection * SpellManager->CurrentSpellInfo.Speed * World->GetDeltaSeconds(), false, &Hit, ETeleportType::None);
}

FVector ASpellProjectile::GetMoveDirection()
{
	if (HasDeterminedDirection || !SpellManager) return CurrentDirection;

	if (SpellManager->Caster)
	{
		HasDeterminedDirection = true;
		CurrentDirection = SpellManager->Caster->GetActorForwardVector();
		return CurrentDirection;
	}

	return CurrentDirection;
}
