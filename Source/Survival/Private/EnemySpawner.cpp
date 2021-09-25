// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "BaseEnemy.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "PoolManager.h"

// Sets default values for this component's properties
UEnemySpawner::UEnemySpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	PoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("PoolManager")));
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

	SpawnEnemy(Level1EnemyClass);
}

void UEnemySpawner::SpawnEnemy(TSubclassOf<ABaseEnemy> EnemyClass)
{
	UWorld* World = GetWorld();

	if (PlayerPawn && World)
	{
		if (EnemyClass)
		{
			FActorSpawnParameters Params;
			FTransform Transform;
			Transform.SetLocation(PlayerPawn->GetActorLocation() + FVector(50.f, 0.f, 40.f));

			AActor* Enemy = PoolManager->GetAvailableActor(EnemyClass);
			if (Enemy)
			{
				Enemy->SetActorLocation(Transform.GetLocation());
				IPoolInterface::Execute_Start(Enemy);
				PoolManager->RegisterActor(Enemy);
			}
			else
			{
				Enemy = World->SpawnActor<ABaseEnemy>(EnemyClass.Get(), Transform, Params);
				IPoolInterface::Execute_Start(Enemy);
				PoolManager->RegisterNewActor(Enemy);
			}
		}
		else { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Enemy class is null! Called from EnemySpawner.cpp -> UpdatePlayerPawn()")); }
	}
	else { UpdatePlayerPawn(); }
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