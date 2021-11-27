// Fill out your copyright notice in the Description page of Project Settings.


#include "StormSpellManager.h"

AStormSpellManager::AStormSpellManager()
{

}

void AStormSpellManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveSpells();
	CheckForCollisions();

}

FVector AStormSpellManager::UpdateDirection(const int Index)
{
	if (SpellInstances[Index].HasGotInitialDirection) return SpellInstances[Index].CurrentDirection;
	if (!Caster) return SpellInstances[Index].CurrentDirection;

	SpellInstances[Index].CurrentDirection = FVector(FMath::FRandRange(-0.4f, 0.4f), FMath::FRandRange(-0.4f, 0.4f), -1.f);
	//SpellInstances[Index].CurrentDirection = Caster->GetActorForwardVector()*-1;

	//SpellInstances[Index].CurrentDirection.X *= 0.75f; // TODO Scale with player speed
	//SpellInstances[Index].CurrentDirection.Y *= 0.75f;

	SpellInstances[Index].CurrentDirection += GetDirectionOffsetForSplit(Index);
	SpellInstances[Index].CurrentDirection.Z = -1.f;
	SpellInstances[Index].CurrentDirection.Normalize();
	SpellInstances[Index].HasGotInitialDirection = true;
	SpellInstances[Index].Transform.SetRotation(SpellInstances[Index].CurrentDirection.Rotation().Quaternion());
	return SpellInstances[Index].CurrentDirection;
}

void AStormSpellManager::UpdateInstanceTransforms()
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
			
			if (SpellInstances[i].Transform.GetLocation().Z <= 0)
			{
				CollideInstance(i, nullptr);
			}
		}
	}
}

FVector AStormSpellManager::GetDirectionOffsetForSplit(const int Index)
{

	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::Printf(TEXT("%i"), OrderIndex));
	const int OrderIndex = SpellInstances[Index].OrderIndex;
	if (OrderIndex == 0) return FVector(0, 0, 0);

	if (SpellModifiers.Contains(SpellModifier::SPLIT))
	{
		if (Caster)
		{
			FVector Forward = Caster->GetActorForwardVector();
			Forward *= FVector(FMath::FRandRange(-0.5, 0.5), FMath::FRandRange(-0.5, 0.5), 0);
			return Forward;

		}

	}
	return FVector(0,0,0);
}
