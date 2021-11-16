// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellStorm.h"
#include "BaseSpellManager.h"

ASpellStorm::ASpellStorm()
{

}

void ASpellStorm::Move()
{
	UWorld* World = GetWorld();
	if (!World || !SpellManager) return;

	FHitResult Hit;
	CurrentDirection = GetMoveDirection();
	AddActorWorldOffset(CurrentDirection * SpellManager->CurrentSpellInfo.Speed * World->GetDeltaSeconds(), false, &Hit, ETeleportType::None);
}

FVector ASpellStorm::GetMoveDirection()
{
	if (HasDeterminedDirection) return CurrentDirection;
	HasDeterminedDirection = true;
	CurrentDirection = FVector(FMath::FRandRange(-0.4, -0.0), FMath::FRandRange(0.0, 0.4f), -1.f);
	return CurrentDirection;
	//return FVector(FMath::FRandRange(-0.5, 0.5), FMath::FRandRange(-0.5, 0.5f), -1.f);
}