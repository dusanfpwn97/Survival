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

AActor* UPoolManager::GetAvailableActor(TSubclassOf<AActor> ActorClass)
{
	if (ActorClass)
	{
		FPooledActors* PooledActorsTemp = PooledActorsMap.Find(ActorClass);
		if (PooledActorsTemp)
		{
			if (PooledActorsTemp->AvailableIndices.Num() == 0)
			{
				return nullptr;
			}
			else
			{
				int32 TempIndex = PooledActorsTemp->AvailableIndices.Last();
				AActor* TempActor = PooledActorsTemp->Actors[TempIndex];
				PooledActorsTemp->AvailableIndices.Remove(TempIndex);
				return TempActor;
			}
		}
		else return nullptr;
	}
	else return nullptr;
}

void UPoolManager::ReleaseActorToPool(AActor* Actor)
{
	if (Actor)
	{
		FPooledActors PooledActorsTemp;
		PooledActorsTemp = PooledActorsMap.FindRef(Actor->GetClass());

		PooledActorsTemp.AvailableIndices.Add(IPoolInterface::Execute_GetPoolIndex(Actor));

		PooledActorsMap.Add(Actor->GetClass(), PooledActorsTemp);

		IPoolInterface::Execute_Reset(Actor);
	}
}

void UPoolManager::RegisterNewActor(AActor* Actor)
{
	if (Actor)
	{
		FPooledActors PooledActorsTemp;
		PooledActorsTemp = PooledActorsMap.FindRef(Actor->GetClass());

		PooledActorsTemp.Actors.Add(Actor);
		IPoolInterface::Execute_SetPoolIndex(Actor, PooledActorsTemp.Actors.Num() - 1);
		PooledActorsMap.Add(Actor->GetClass(), PooledActorsTemp);
	}
}

void UPoolManager::RegisterActor(AActor* Actor)
{
	if (Actor)
	{
		FPooledActors PooledActorsTemp;
		PooledActorsTemp = PooledActorsMap.FindRef(Actor->GetClass());

		PooledActorsTemp.AvailableIndices.Remove(IPoolInterface::Execute_GetPoolIndex(Actor));
		
		PooledActorsMap.Add(Actor->GetClass(), PooledActorsTemp);
	}
}
