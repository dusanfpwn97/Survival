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
		CurrentDirection = SpellManager->Caster->GetActorForwardVector();
	}
	CurrentDirection += GetDirectionForSplit();
	CurrentDirection.Normalize();

	HasDeterminedDirection = true;
	return CurrentDirection;
}

void ASpellProjectile::Start_Implementation()
{
	Super::Start_Implementation();
}

void ASpellProjectile::Reset_Implementation()
{
	Super::Reset_Implementation();
}

FVector ASpellProjectile::GetDirectionForSplit()
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::Printf(TEXT("%i"), OrderIndex));

	if (OrderIndex == 0 || !SpellManager) return FVector(0, 0, 0);

	if (SpellManager->SpellModifiers.Contains(SpellModifier::SPLIT))
	{
		if (SpellManager->Caster)
		{
			FVector Forward = SpellManager->Caster->GetActorForwardVector();
			FVector Right = SpellManager->Caster->GetActorRightVector();
			FVector Result;
			float LerpAlpha = 0.f;
			if (OrderIndex % 2 == 0)
			{
				//Result = Forward + (Right * OrderIndex) * OrderIndex;
				LerpAlpha = 1.f / 8.f * OrderIndex;
				Result = FMath::Lerp(Forward, Right, LerpAlpha);
			}
			else
			{
				LerpAlpha = 1.f / 8.f * (OrderIndex+1);
				//Result = Forward + (Right * -1 * (OrderIndex + 1) * OrderIndex+1) ;
				Result = FMath::Lerp(Forward, Right*-1, LerpAlpha);
			}
			
			Result.Normalize();
			return Result;
		}

	}
	return FVector(0, 0, 0);
}