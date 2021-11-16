// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellFlick.h"
#include "BaseSpellManager.h"

ASpellFlick::ASpellFlick()
{

}

void ASpellFlick::Move()
{
	UWorld* World = GetWorld();
	if (!World || !SpellManager) return;

	FHitResult Hit;
	CurrentDirection = GetMoveDirection();
	AddActorWorldOffset(CurrentDirection * SpellManager->CurrentSpellInfo.Speed * World->GetDeltaSeconds(), false, &Hit, ETeleportType::None);
}

FVector ASpellFlick::GetMoveDirection()
{
	if (!TargetActor)
	{
		if (CurrentDirection.IsNearlyZero(0.01))
		{
			CurrentDirection = SpellManager->Caster->GetActorForwardVector();
		}
		return CurrentDirection;
	}

	if (TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
	{
		if (ICombatInterface::Execute_GetIsAlive(TargetActor))
		{
			FVector NewDirection = TargetActor->GetActorLocation() - GetActorLocation();
			NewDirection.Normalize();
			return NewDirection;
		}
		else return CurrentDirection;
	}
	return CurrentDirection;
}