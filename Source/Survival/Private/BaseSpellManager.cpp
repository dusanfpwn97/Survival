// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpellManager.h"
#include "BaseSpell.h"
#include "Kismet/GameplayStatics.h"
#include "CombatInterface.h"
#include "PoolManager.h"

// Sets default values for this component's properties
UBaseSpellManager::UBaseSpellManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	SpellPoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("SpellPoolManager")));
}


// Called when the game starts
void UBaseSpellManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetWorld()->GetTimerManager().SetTimer(MainSpellCastTimerHandle, this, &UBaseSpellManager::CastSpell, 0.5f/*FMath::RandRange(2.2f, 5.3f)*/, true);
}

// Called every frame
void UBaseSpellManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBaseSpellManager::CastSpell()
{
	//SpellInfo.SpellClass.Get();
	UClass* ClassToSpawn = SpellInfo.SpellClass.Get();
	if (!ClassToSpawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Spell class is null! BaseSpellManager.Cpp -> CastSpell"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	if (!PlayerPawn) UpdatePlayerPawn();
	if (!PlayerPawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Cant get player so cannot cast spell"));
		return;
	}

	AActor* SpellToCast = SpellPoolManager->GetAvailableActor(ClassToSpawn);

	if (!SpellToCast) GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Yellow, TEXT("Spell couldn't be spawned. Shouldn't happen! EnemySpawner.cpp -> SpawnEnemy()"));

	//ICombatInterface::Execute_SetTarget(Enemy, PlayerPawn); */
	FVector Location = ICombatInterface::Execute_GetSpellCastLocation(PlayerPawn);
	SpellToCast->SetActorLocation(Location);
}

void UBaseSpellManager::UpdatePlayerPawn()
{
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		if (!PlayerPawn) { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Cannot find player pawn! Called from BaseSpellManager.cpp -> UpdatePlayerPawn()")); } }
	}
	else { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("World is null! Called from EnemySpawner.cpp -> UpdatePlayerPawn()")); } }
}


