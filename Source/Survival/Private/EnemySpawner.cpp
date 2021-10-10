// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "BaseEnemy.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "CombatInterface.h"
#include "PoolManager.h"
#include "HelperFunctions.h"

// Sets default values for this component's properties
UEnemySpawner::UEnemySpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	EnemySpawnPoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("EnemySpawnPoolManager")));
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
	float s = FMath::FRand();
	if (s < 0.01)
	{
		SpawnEnemy(SoftLevel1EnemyClass.Get());
	}


}

void UEnemySpawner::SpawnEnemy(TSubclassOf<ABaseEnemy> EnemyClass)
{
	if (spawnnum > 50 || !IsSpawnEnabled) return;

	UWorld* World = GetWorld();
	if (!World) return;
	if (!EnemyClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Enemy class is null! EnemySpawner.cpp -> SpawnEnemy()"));
		return;
	}

	if (!PlayerPawn) UpdatePlayerPawn();
	if (!PlayerPawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Cant get player so enemy cant be spawned"));
		return;
	}

	AActor* Enemy = EnemySpawnPoolManager->GetAvailableActor(EnemyClass);

	if (!Enemy) GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Yellow, TEXT("Enemy couldn't be spawned. Shouldn't happen! EnemySpawner.cpp -> SpawnEnemy()"));

	Enemy->SetActorLocation(UHelperFunctions::GetRandomPointInCircle(PlayerPawn->GetActorLocation() + FVector(0.f, 0.f, 45.f), 1500.f));
	
	ICombatInterface::Execute_SetTarget(Enemy, PlayerPawn);
	spawnnum++;
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
/*
void UEnemySpawner::ReleaseToPool_Implementation(AActor* Actor)
{
	//EnemyPool.Add(Actor);
	//spawnnum--;
	//ActiveEnemies.Remove(Actor); // TODO BAD PERFORMANCE

}*/