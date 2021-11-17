// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellShield.h"
#include "BaseSpellManager.h"

void ASpellShield::Move()
{
	UWorld* World = GetWorld();
	if (!World || !SpellManager) return;


	//FHitResult Hit;
	//CurrentDirection = GetMoveDirection();
	//AddActorWorldOffset(CurrentDirection * SpellManager->CurrentSpellInfo.Speed * World->GetDeltaSeconds(), false, &Hit, ETeleportType::None);
	if (SpellManager->Caster)
	{
		SetActorLocation(SpellManager->Caster->GetActorLocation(), false);
	}
}
