// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "BaseEnemy.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "CombatInterface.h"
#include "PoolManager.h"
#include "HelperFunctions.h"
#include "FakeShadowDistributer.h"
#include "Components/SkeletalMeshComponent.h"
#include "BasePlayerPawn.h"

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
	SpawnFakeShadowDistributer();
	UpdatePlayerPawn();
	SoftLevel1EnemyClass.LoadSynchronous();
	SoftLevel2EnemyClass.LoadSynchronous();
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(CommonEnemySpawnTimer, this, &UEnemySpawner::HandleSpawning, 0.53f, true);
		World->GetTimerManager().SetTimer(DebugTimerHandle, this, &UEnemySpawner::DebugValues, 0.5f, true);
	}
	
}

void UEnemySpawner::SpawnFakeShadowDistributer()
{
	// Shadow distributer already exists
	if (FakeShadowDistributer) return;
	FTransform TempTransform;
	TempTransform.SetLocation(FVector(0, 0, -250));
	TempTransform.SetScale3D(FVector(1, 1, 1));
	TempTransform.SetRotation(FRotator(-90,0,0).Quaternion());
	FActorSpawnParameters Params;

	UWorld* World = GetWorld();
	if (World)
	{
		FakeShadowDistributer = World->SpawnActor<AFakeShadowDistributer>(AFakeShadowDistributer::StaticClass(), TempTransform, Params);
	}
}

// Called every frame
void UEnemySpawner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CachedAliveSpawns = GetAliveSpawns();
}

TArray<AActor*> UEnemySpawner::GetAllSpawns()
{
	return EnemySpawnPoolManager->AllSpawnedActors;
}

TArray<AActor*> UEnemySpawner::GetAliveSpawns()
{
	TArray<AActor*> AliveSpawns;
	TArray<AActor*> AllSpawns = EnemySpawnPoolManager->AllSpawnedActors;

	for (AActor* Actor : AllSpawns)
	{
		ICombatInterface* TempInterface = Cast<ICombatInterface>(Actor);
		if (TempInterface)
		{
			if (TempInterface->GetIsAlive())
			{
				AliveSpawns.Push(Actor);
			}
		}
	}
	return AliveSpawns;
}

void UEnemySpawner::SpawnEnemy(TSoftClassPtr<ABaseEnemy> EnemyClassToSpawn)
{
	//return;
	UClass* EnemyClass = EnemyClassToSpawn.Get();
	//if (spawnnum > 50) return;
	if (!IsSpawnEnabled) return;

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

	bool IsCached;
	AActor* Enemy = EnemySpawnPoolManager->GetAvailableActor(EnemyClass, IsCached);

	if (!Enemy) GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Yellow, TEXT("Enemy couldn't be spawned. Shouldn't happen! EnemySpawner.cpp -> SpawnEnemy()"));
	FVector SpawnLoc = PlayerPawn->GetActorLocation();
	SpawnLoc.Z = 2.f;
	Enemy->SetActorLocation(UHelperFunctions::GetRandomPointInCircle(SpawnLoc, 1500.f));
	
	ICombatInterface* TempInterface = Cast<ICombatInterface>(Enemy);
	TempInterface->SetTarget(PlayerPawn);
	spawnnum++;

	if (FakeShadowDistributer && !IsCached)
	{
		ABaseEnemy* EnemyActor = Cast<ABaseEnemy>(Enemy);
		if (EnemyActor)
		{
			FakeShadowDistributer->AssignNewShadow(Enemy, FVector(0, 0, 0), EnemyActor->GetSkeletalMesh(), FName("ShadowSocket"));
			EnemyActor->Spawner = this;
		}
	}
}

void UEnemySpawner::HandleSpawning()
{
	float Rand = FMath::FRand();

	if (Rand > 0.5)
	{
		//SpawnEnemy(SoftLevel1EnemyClass);
	}
	else
	{
		SpawnEnemy(SoftLevel2EnemyClass);
	}
}

void UEnemySpawner::DebugValues()
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("Spawned Enemies: %i"), EnemySpawnPoolManager->AllSpawnedActors.Num()));
}

void UEnemySpawner::UpdatePlayerPawn()
{
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerPawn = Cast<ABasePlayerPawn>(UGameplayStatics::GetPlayerPawn(World, 0));
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