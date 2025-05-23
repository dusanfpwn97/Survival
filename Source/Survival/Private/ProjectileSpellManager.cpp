// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSpellManager.h"

AProjectileSpellManager::AProjectileSpellManager()
{

}

void AProjectileSpellManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveSpells();
	CheckForCollisions();
	// ...

}

void AProjectileSpellManager::UpdateDirection(const int Index)
{
	//return FVector(1, 0, 0);

	if (SpellInstances[Index].HasGotInitialDirection) return;
	if (!Caster) return;

	SpellInstances[Index].CurrentDirection = Caster->GetActorForwardVector();

	SpellInstances[Index].CurrentDirection += GetDirectionForSplit(Index);
	SpellInstances[Index].CurrentDirection.Normalize();
	SpellInstances[Index].HasGotInitialDirection = true;
	SpellInstances[Index].Transform.SetRotation(SpellInstances[Index].CurrentDirection.Rotation().Quaternion());
}

FVector AProjectileSpellManager::GetDirectionForSplit(const int Index)
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::Printf(TEXT("%i"), OrderIndex));
	const int OrderIndex = SpellInstances[Index].OrderIndex;
	if (OrderIndex == 0) return FVector(0, 0, 0);

	if (SpellModifiers.Contains(SpellModifier::SPLIT))
	{
		if (Caster)
		{
			FVector Forward = Caster->GetActorForwardVector();
			FVector Right = Caster->GetActorRightVector();
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
				LerpAlpha = 1.f / 8.f * (OrderIndex + 1);
				//Result = Forward + (Right * -1 * (OrderIndex + 1) * OrderIndex+1) ;
				Result = FMath::Lerp(Forward, Right * -1, LerpAlpha);
			}

			Result.Normalize();
			return Result;
		}

	}
	return FVector(0, 0, 0);
}

void AProjectileSpellManager::UpdateInstanceTransforms()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			UpdateDirection(i);
			SpellInstances[i].Velocity += (SpellInstances[i].CurrentDirection * CurrentSpellInfo.Speed * 0.06);
			SpellInstances[i].Velocity = SpellInstances[i].Velocity.GetClampedToMaxSize(1200.f);
			SpellInstances[i].Transform.SetLocation(SpellInstances[i].Transform.GetLocation() += SpellInstances[i].Velocity * World->DeltaTimeSeconds);
		}
	}
}
