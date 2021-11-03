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
	UWorld* World = GetWorld();
	if (World)
	{
		GetWorld()->GetTimerManager().SetTimer(CommonEnemySpawnTimer, this, &UEnemySpawner::SpawnEnemy, 0.8f, true);
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

}

TArray<AActor*> UEnemySpawner::GetAllSpawns()
{
	return EnemySpawnPoolManager->AllSpawnedActors;
}

void UEnemySpawner::SpawnEnemy()
{
	//return;
	UClass* EnemyClass = SoftLevel1EnemyClass.Get();
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
	SpawnLoc.Z = 90;
	Enemy->SetActorLocation(UHelperFunctions::GetRandomPointInCircle(SpawnLoc, 1500.f));
	
	ICombatInterface::Execute_SetTarget(Enemy, PlayerPawn);
	spawnnum++;

	if (FakeShadowDistributer && !IsCached)
	{
		ABaseEnemy* EnemyActor = Cast<ABaseEnemy>(Enemy);
		if (EnemyActor)
		{
			FakeShadowDistributer->AssignNewShadow(Enemy, FVector(0, 0, 0), EnemyActor->GetSkeletalMesh(), FName("ShadowSocket"));
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
/*
void UEnemySpawner::ReleaseToPool_Implementation(AActor* Actor)
{
	//EnemyPool.Add(Actor);
	//spawnnum--;
	//ActiveEnemies.Remove(Actor); // TODO BAD PERFORMANCE

}*/