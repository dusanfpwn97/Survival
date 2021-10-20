// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpellManager.h"
#include "BaseSpell.h"
#include "Kismet/GameplayStatics.h"
#include "CombatInterface.h"
#include "PoolManager.h"
#include "BasePlayerPawn.h"
#include "NiagaraSystem.h"
#include "HelperFunctions.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values for this component's properties
UBaseSpellManager::UBaseSpellManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	SpellPoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("SpellPoolManager")));
}

FSpellInfo UBaseSpellManager::GetSpellInfo()
{
	return SpellInfo;
}

// Called when the game starts
void UBaseSpellManager::BeginPlay()
{

	Super::BeginPlay();
	Caster = GetOwner();

	
	SpellInfo.TargetMode = TargetMode::CLOSEST;
	SpellInfo.Element = Element::FIRE;
	SpellInfo.CastType = CastType::FLICK;
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

	if (!SpellToCast)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Spell couldn't be spawned. Shouldn't happen! EnemySpawner.cpp -> SpawnEnemy()"));
		return;
	}

	FVector Location = ICombatInterface::Execute_GetSpellCastLocation(Caster);
	ICombatInterface::Execute_SetSpellManager(SpellToCast, this);
	if (InitialTarget)
	{
		ICombatInterface::Execute_SetTarget(SpellToCast, InitialTarget);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("No target"));
	}

	IPoolInterface::Execute_SetSpawner(SpellToCast, SpellPoolManager);
	SpellToCast->SetActorLocation(Location);
}

AActor* UBaseSpellManager::GetActorForTarget()
{
	if (!Caster) return nullptr;
	if (!Caster->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) return nullptr;

	if (SpellInfo.TargetMode == TargetMode::CLOSEST)
	{
		return ICombatInterface::Execute_GetClosestEnemy(Caster);
	}

	return nullptr;
}

void UBaseSpellManager::UpdateSpellClass()
{
	SpellClassToSpawn = SpellInfo.SpellClass.LoadSynchronous();
	if (!SpellClassToSpawn)
	{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Spell class is null! BaseSpellManager.Cpp -> CastSpell"));
			return;
	}
}

UNiagaraSystem* UBaseSpellManager::GetNiagaraSystem(Element Element, CastType CastType, SpellFXType SpellFXType)
{
	if (CachedParticles.Contains(SpellFXType))
	{
		return CachedParticles.FindRef(SpellFXType);
	}

	FString NSPath = "NiagaraSystem'/Game/_Core/Spells/NiagaraParticles/NS_";
	
	NSPath += UHelperFunctions::GetElementName(Element);
	NSPath += UHelperFunctions::GetCastTypeName(CastType);
	NSPath += UHelperFunctions::GetSpellFXTypeName(SpellFXType);
	NSPath += ".NS_";
	NSPath += UHelperFunctions::GetElementName(Element);
	NSPath += UHelperFunctions::GetCastTypeName(CastType);
	NSPath += UHelperFunctions::GetSpellFXTypeName(SpellFXType);
	NSPath += "'";

	UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(nullptr, *NSPath);

	if (NS)
	{
		CachedParticles.Add(SpellFXType, NS);
		return NS;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Failed to find Niagara System %s"), *NSPath));
		SET_WARN_COLOR(COLOR_RED);
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Niagara System %s"), *NSPath);
		CLEAR_WARN_COLOR();
		return nullptr;
	}
}

void UBaseSpellManager::SpawnHitParticle(FVector Location)
{
	UNiagaraSystem* System = GetNiagaraSystem(SpellInfo.Element, SpellInfo.CastType, SpellFXType::ON_HIT);

	
	UWorld* World = GetWorld();
	if (!World) return;

	if (System)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, System, Location, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::AutoRelease, true);
	}
	else
	{
		FString ss = "";
		ss += UHelperFunctions::GetElementName(SpellInfo.Element);
		ss += UHelperFunctions::GetCastTypeName(SpellInfo.CastType);
		ss += UHelperFunctions::GetSpellFXTypeName(SpellFXType::ON_HIT);

		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Failed to spawn Niagara System %s"), *ss));
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Niagara System %s"), *ss);
	}
}

AActor* UBaseSpellManager::GetCaster() const
{
	return Caster;
}
