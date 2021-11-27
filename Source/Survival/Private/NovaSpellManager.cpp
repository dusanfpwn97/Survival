// Fill out your copyright notice in the Description page of Project Settings.


#include "NovaSpellManager.h"

ANovaSpellManager::ANovaSpellManager()
{
	SpellLifetime = 0.25f;
}

void ANovaSpellManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveSpells();
	CheckForCollisions();

}


void ANovaSpellManager::UpdateInstanceTransforms()
{
	UWorld* World = GetWorld();
	if (!World) return;


	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			FVector PrevScale = SpellInstances[i].Transform.GetScale3D();
			PrevScale.X += World->GetDeltaSeconds() * 20;
			PrevScale.Y += World->GetDeltaSeconds() * 20;
			PrevScale.Z += World->GetDeltaSeconds() * 20;
			SpellInstances[i].Transform.SetScale3D(PrevScale);
		}
	}
}