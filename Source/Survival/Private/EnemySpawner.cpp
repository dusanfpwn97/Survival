// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "BaseEnemy.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UEnemySpawner::UEnemySpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	//UpdatePlayerPawn();
}

// Called every frame
void UEnemySpawner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//SpawnEnemy(Level1EnemyClass);
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
			World->SpawnActor<ABaseEnemy>(EnemyClass.Get(), Transform, Params);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Enemy class is null! Called from EnemySpawner.cpp -> UpdatePlayerPawn()"));
		}
	}
	else
	{
		UpdatePlayerPawn();
	}

}

void UEnemySpawner::UpdatePlayerPawn()
{
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Cannot find player pawn! Called from EnemySpawner.cpp -> UpdatePlayerPawn()"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("World is null! Called from EnemySpawner.cpp -> UpdatePlayerPawn()"));
	}
}