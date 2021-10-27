// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolManager.h"
#include "PoolInterface.h"
#include "PoolInterface.h"
#include "Kismet/GameplayStatics.h"

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

AActor* UPoolManager::GetAvailableActor(TSubclassOf<AActor> ActorClass, bool &IsCached)
{
	if (ActorClass)
	{
		AActor* ActorToGet;
		FPooledActors PooledActorsTemp = PooledActorsMap.FindRef(ActorClass);
		if (PooledActorsTemp.Actors.Num() > 0)
		{
			//ActorToGet = PooledActorsTemp.Actors.Last();

			ActorToGet = PooledActorsTemp.Actors.Pop();
			IsCached = true;
			//PooledActorsTemp.Actors.RemoveAt(PooledActorsTemp.Actors.Num() - 1);
			PooledActorsMap.Emplace(ActorToGet->GetClass(), PooledActorsTemp);
			IPoolInterface::Execute_Start(ActorToGet);

			return ActorToGet;
		}
		else
		{
			UWorld* World = GetWorld();
			if (!World) return nullptr;

			FTransform Transform;
			FActorSpawnParameters Params;
			
			Transform.SetLocation(FVector(0.f, 0.f, 500.f));

			ActorToGet = World->SpawnActor<AActor>(ActorClass, Transform, Params);
			AllSpawnedActors.Add(ActorToGet);
			IPoolInterface::Execute_Start(ActorToGet);
			IPoolInterface::Execute_SetSpawner(ActorToGet, this);
			IsCached = false;
			return ActorToGet;
		}
	}
	else return nullptr;
}

void UPoolManager::ReleaseToPool_Implementation(AActor* Actor)
{
	if (Actor)
	{
		FPooledActors PooledActorsTemp = PooledActorsMap.FindRef(Actor->GetClass());

		PooledActorsTemp.Actors.Push(Actor);

		PooledActorsMap.Emplace(Actor->GetClass(), PooledActorsTemp);
	}
}