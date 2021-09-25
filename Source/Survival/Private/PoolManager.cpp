// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolManager.h"
#include "PoolInterface.h"
#include "PoolInterface.h"

// Sets default values for this component's properties
UPoolManager::UPoolManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPoolManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPoolManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UPoolManager::RemoveFromPool(TSubclassOf<AActor> ActorClass)
{
	if (ActorClass)
	{
		FPooledActors* Actors;
		Actors = PooledActors.Find(ActorClass);

		if (Actors->AvailableIndices.Num() == 0) { return -1; }
		else
		{
			int32 TempIndex = Actors->AvailableIndices.Last();
			Actors->AvailableIndices.RemoveAt(TempIndex);
			return TempIndex;
		};
	}
	else return -1;

}

void UPoolManager::ReleaseActorToPool(AActor* Actor)
{
	FPooledActors* Actors;
	Actors = PooledActors.Find(Actor->GetClass());

	Actors->AvailableIndices.Add(IPoolInterface::Execute_GetPoolIndex(Actor));
}

void UPoolManager::RegisterNewActor(AActor* Actor)
{
	if (Actor)
	{
		FPooledActors* Actors;
		Actors = PooledActors.Find(Actor->GetClass());

		Actors->Actors.Add(Actor);

		//int32 Index = IPoolInterface::Execute_GetPoolIndex(Actor);
		//Actors->AvailableIndices
	}
}

