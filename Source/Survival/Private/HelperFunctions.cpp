// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions.h"
#include "CombatInterface.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/KismetMathLibrary.h"


FVector UHelperFunctions::GetRandomPointInCircle(FVector Center, float Radius)
{
	FVector Location;
	float rand = FMath::FRand();
	float RandPoint = rand * 2 * 3.1415f;
	Location.X = Center.X + Radius * FMath::Cos(RandPoint);
	Location.Y = Center.Y + Radius * FMath::Sin(RandPoint);
	Location.Z = Center.Z;
	return Location;
}

AActor* UHelperFunctions::GetClosestActor(TArray<AActor*> Actors, FVector ReferenceLocation)
{
	if (Actors.Num() == 0) return nullptr;
	if (Actors.Num() == 1) return Actors[0];

	int32 ClosestActorIndex = 0;
	float ClosestDistance = 999999.f;

	for (int i = 0; i <= Actors.Num() - 1; i++)
	{
		float TempDistance = FVector::Distance(Actors[i]->GetActorLocation(), ReferenceLocation);
		if(ClosestDistance > TempDistance)
		{	
			ClosestDistance = TempDistance;
			ClosestActorIndex = i;
		}
	}
	
	return Actors[ClosestActorIndex];
}

TArray<AActor*> UHelperFunctions::GetClosestActors(TArray<AActor*> Actors, FVector ReferenceLocation, int32 NumOfActors)
{
	if (Actors.Num() == 0) return TArray<AActor*>();
	if (Actors.Num() <= NumOfActors) return Actors;

	TArray<float> Distances;
	for (int i = 0; i < Actors.Num(); i++)
	{
		Distances.Add(FVector::Distance(Actors[i]->GetActorLocation(), ReferenceLocation));
	}

	TArray<AActor*> ClosestActors;

	for (int i = 0; i < Actors.Num(); i++)
	{
		int32 MinIndex;
		float MinValue;

		UKismetMathLibrary::MinOfFloatArray(Distances, MinIndex, MinValue);
		Distances.RemoveAt(MinIndex);
		ClosestActors.Add(Actors[MinIndex]);

			
		if (ClosestActors.Num() == NumOfActors)
		{
			return ClosestActors;
		}
	}

	return TArray<AActor*>();
}


TArray<AActor*> UHelperFunctions::GetAllAliveActors(TArray<AActor*> ActorsToCheck)
{
	TArray<AActor*> AliveActors;

	for (AActor* Actor : ActorsToCheck)
	{
		if (Actor)
		{
			if (Actor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
			{
				ICombatInterface* TempInterface = Cast<ICombatInterface>(Actor);
				
				//interfacess->GetIsAliveCpp_Implementation
				//bool ss = ICombatInterface::GetIsAliveCpp(Actor);
				
				if (TempInterface->GetIsAlive())
				{
					AliveActors.Add(Actor);
				}
			}
		}
	}
	return AliveActors;
}
