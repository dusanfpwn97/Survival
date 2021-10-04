// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "BaseEnemy.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "CombatInterface.h"
#include "PoolManager.h"

// Sets default values for this component's properties
UEnemySpawner::UEnemySpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//PoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("PoolManager")));
}

void UEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	UpdatePlayerPawn();
}

// Called every frame
void UEnemySpawner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (FMath::RandBool())
	{
		SpawnEnemy(Level1EnemyClass);
	}


}

void UEnemySpawner::SpawnEnemy(TSubclassOf<ABaseEnemy> EnemyClass)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!PlayerPawn) UpdatePlayerPawn();
	if (!PlayerPawn) return;

	if (!EnemyClass) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Enemy class is null! EnemySpawner.cpp -> SpawnEnemy()")); }
	if (spawnnum > 100) return;
	else
	{
		FActorSpawnParameters Params;
		FTransform Transform;
		Transform.SetLocation(PlayerPawn->GetActorLocation() + FVector(FMath::RandRange(-500.f, 500.f), FMath::RandRange(-500.f, 500.f), 20.f));
		AActor* Enemy;

		if (EnemyPool.Num() == 0)
		{
			Enemy = World->SpawnActor<ABaseEnemy>(EnemyClass.Get(), Transform, Params);
			IPoolInterface::Execute_SetSpawner(Enemy, this);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spawned"));
		}
		else
		{
			Enemy = EnemyPool.Last();
			EnemyPool.RemoveAt(EnemyPool.Num() - 1);
		}
		
		if (Enemy)
		{
			Enemy->SetActorLocation(Transform.GetLocation());
			ICombatInterface::Execute_SetTarget(Enemy, PlayerPawn);
			IPoolInterface::Execute_Start(Enemy);
			spawnnum++;
			//ActiveEnemies.Add(Enemy);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not spawn enemy! Should not happen!!! EnemySpawner.cpp -> SpawnEnemy()"));
		}
	}
}

void UEnemySpawner::UpdatePlayerPawn()
{
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		if (!PlayerPawn) { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Cannot find player pawn! Called from EnemySpawner.cpp -> UpdatePlayerPawn()")); } }
	}
	else { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("World is null! Called from EnemySpawner.cpp -> UpdatePlayerPawn()")); } }
}

void UEnemySpawner::ReleaseToPool_Implementation(AActor* Actor)
{
	EnemyPool.Add(Actor);
	spawnnum--;
	//ActiveEnemies.Remove(Actor); // TODO BAD PERFORMANCE

}
/*
TArray<AActor*> UEnemySpawner::GetSpawnedEnemies_Implementation()
{
	return ActiveEnemies;
}*/
