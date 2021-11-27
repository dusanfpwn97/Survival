// Fill out your copyright notice in the Description page of Project Settings.


#include "NovaSpellManager.h"
#include "CombatInterface.h"

ANovaSpellManager::ANovaSpellManager()
{
	SpellLifetime = 4.f;
}

void ANovaSpellManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveSpells();
	CheckForCollisions();

}

void ANovaSpellManager::CheckForCollisions()
{
	// TODO check how much time this takes
	if (!Caster) return;

	ICombatInterface* TempInterface = Cast<ICombatInterface>(Caster);
	TArray<AActor*> ActorsToCheck = TempInterface->GetAliveEnemies();

	//Single threaded

	for (int j = 0; j < SpellInstances.Num(); j++)
	{
		for (int i = 0; i < ActorsToCheck.Num(); i++)
		{
			const FVector SpellLoc = SpellInstances[j].Transform.GetLocation();

			AActor* TempActor = ActorsToCheck[i];
			if (TempActor != this && TempActor != Caster && TempActor)// && !CollidedActors.Contains(Actor) 
			{
				float Dist = FVector::Dist(SpellLoc, TempActor->GetActorLocation() + FVector(0.f, 0.f, 100.f));

				if (Dist < CurrentSpellInfo.Radius)
				{
					CollideInstance(j, TempActor);
				}
			}
		}
	}
}

void ANovaSpellManager::UpdateInstanceTransforms()
{
	UWorld* World = GetWorld();
	if (!World) return;


	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			if (SpellInstances[i].CurrentRadius < CurrentSpellInfo.Radius) // max radius
			{
				SpellInstances[i].CurrentRadius += World->GetDeltaSeconds() * 1100;

				float Diameter = FMath::Sqrt(SpellInstances[i].CurrentRadius / 3.14f);

				GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("%s"), *FString::SanitizeFloat(SpellInstances[i].CurrentRadius)));

				SpellInstances[i].Transform.SetScale3D(FVector(Diameter, Diameter, Diameter));
			}

			else
			{
				ResetInstance(i);
			}
		}	
	}
}