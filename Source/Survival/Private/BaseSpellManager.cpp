// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpellManager.h"
#include "BaseSpell.h"
#include "Kismet/GameplayStatics.h"
#include "CombatInterface.h"
#include "PoolManager.h"
#include "BasePlayerPawn.h"
//#include "CombatComponent.h"


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
	Caster = GetOwner();

	SpellInfo.TargetMode = TargetMode::CLOSEST;
	// ...
	GetWorld()->GetTimerManager().SetTimer(MainSpellCastTimerHandle, this, &UBaseSpellManager::CastSpell, 0.3f/*FMath::RandRange(2.2f, 5.3f)*/, true);
}

// Called every frame
void UBaseSpellManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBaseSpellManager::CastSpell()
{

	if (!SpellClassToSpawn)
	{
		UpdateSpellClass();
		if (!SpellClassToSpawn)
		{
			return;
		}
	}

	UWorld* World = GetWorld();
	if (!World) return;
	if (!Caster) return;
	{
		Caster = GetOwner();
		if (!Caster)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Caster is nullptr, so cannot cast spell"));
			return;
		}
	}

	AActor* InitialTarget = GetActorForTarget();

	// Don't cast a spell if there is no target and Target mode is not NONE!
	if (!InitialTarget && SpellInfo.TargetMode != TargetMode::NONE) return;
	
	AActor* SpellToCast = SpellPoolManager->GetAvailableActor(SpellClassToSpawn);

	if (!SpellToCast) GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Yellow, TEXT("Spell couldn't be spawned. Shouldn't happen! EnemySpawner.cpp -> SpawnEnemy()"));

	FVector Location = ICombatInterface::Execute_GetSpellCastLocation(Caster);
	ICombatInterface::Execute_SetSpellManager(SpellToCast, this);

	ICombatInterface::Execute_SetTarget(SpellToCast, InitialTarget);
	SpellToCast->SetActorLocation(Location);
}

AActor* UBaseSpellManager::GetActorForTarget()
{
	if (!Caster) return nullptr;
	if (!Caster->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) return nullptr;

	if (SpellInfo.TargetMode == TargetMode::CLOSEST)
	{
		//AActor* sfd = ICombatInterface::Execute_GetClosestEnemy(Caster);
		return ICombatInterface::Execute_GetClosestEnemy(Caster);

	}

	return nullptr;
}

void UBaseSpellManager::UpdateSpellClass()
{
	if (SpellInfo.SpellClass.IsValid())
	{
		SpellClassToSpawn = SpellInfo.SpellClass.LoadSynchronous();
		if (!SpellClassToSpawn)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Spell class is null! BaseSpellManager.Cpp -> CastSpell"));
			return;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No spell set! BaseSpellManager.Cpp -> CastSpell"));
		return;
	}
}



