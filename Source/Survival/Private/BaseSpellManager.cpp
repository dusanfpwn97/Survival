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
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values for this component's properties
UBaseSpellManager::UBaseSpellManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	SpellPoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("SpellPoolManager")));

	SetVFXDataTable();
}

// Called when the game starts
void UBaseSpellManager::BeginPlay()
{

	Super::BeginPlay();
	Caster = GetOwner();
}

void UBaseSpellManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBaseSpellManager::CastSpell()
{
	/*
	if (!SpellClassToSpawn)
	{
		UpdateSpellClass();
		if (!SpellClassToSpawn)
		{
			return;
		}
	}*/

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
	if (!InitialTarget && CurrentSpellInfo.TargetMode != TargetMode::NONE) return;
	
	bool IsCached;
	AActor* SpellToCast = SpellPoolManager->GetAvailableActor(ABaseSpell::StaticClass(), IsCached);

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

	if (CurrentSpellInfo.TargetMode == TargetMode::CLOSEST)
	{
		return ICombatInterface::Execute_GetClosestEnemy(Caster);
	}

	return nullptr;
}

void UBaseSpellManager::InitSpellManager(FSpellInfo NewSpellInfo)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo = NewSpellInfo;
	UWorld* World = GetWorld();
	if (!World) return;
	World->GetTimerManager().ClearTimer(MainSpellCastTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(MainSpellCastTimerHandle, this, &UBaseSpellManager::CastSpell, CurrentSpellInfo.Cooldown, true);

}

void UBaseSpellManager::MarkAllSpellsForDestruction()
{
	TArray<AActor*> SpawnedActors = SpellPoolManager->AllSpawnedActors;
	while (SpawnedActors.Num() != 0)
	{
		if (SpawnedActors.Last())
		{
			ABaseSpell* Spell = Cast<ABaseSpell>(SpawnedActors.Last());
			if (Spell)
			{
				Spell->IsMarkedForDestruction = true;
				SpawnedActors.RemoveAt(SpawnedActors.Num() - 1);
			}
		}
	}
	CachedParticles.Empty();
	SpellPoolManager->AllSpawnedActors.Empty();
}

void UBaseSpellManager::DestroySpellManager()
{
	MarkAllSpellsForDestruction();
}

void UBaseSpellManager::UpdateCastType(CastType NewCastType)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo.CastType = NewCastType;
}

void UBaseSpellManager::UpdateSpellModifier(SpellModifier NewSpellModifier)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo.SpellModifier = NewSpellModifier;
}

void UBaseSpellManager::UpdateSpellClass()
{
	SpellClassToSpawn = CurrentSpellInfo.SpellClass.LoadSynchronous();
	if (!SpellClassToSpawn)
	{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Spell class is null! BaseSpellManager.Cpp -> CastSpell"));
			return;
	}
}

UNiagaraSystem* UBaseSpellManager::GetNiagaraSystem(Element Element, CastType CastType, SpellFXType SpellFXType)
{

	if (CachedParticles.Contains(SpellFXType)) return CachedParticles.FindRef(SpellFXType);

	if (!VFX_DataTable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Couldn't find Datatable for Spell VFX")));
		return nullptr;
	}

	UNiagaraSystem* NS = nullptr;

	for (auto it : VFX_DataTable->GetRowMap())
	{
		
		FSpellVFXInfo* SpellVFXInfo = (FSpellVFXInfo*)(it.Value);
		//FSpellVFXInfo* SpellVFXInfo = Cast<FSpellVFXInfo>(it.Value);
		if (SpellVFXInfo)
		{
			if (SpellVFXInfo->Binding.Element == Element && SpellVFXInfo->Binding.CastType == CastType)
			{
				if (SpellFXType == SpellFXType::ON_SPAWN)
				{
					NS = SpellVFXInfo->SpawnFX.LoadSynchronous();
				}
				else if (SpellFXType == SpellFXType::MAIN)
				{
					NS = SpellVFXInfo->MainFX.LoadSynchronous();
				}
				else if (SpellFXType == SpellFXType::ON_HIT)
				{
					NS = SpellVFXInfo->HitFX.LoadSynchronous();
				}

				if (NS)
				{
					CachedParticles.Add(SpellFXType, NS);
					return NS;
				}
			}
		}
	}

	if (!NS)
	{

		FString sss = UHelperFunctions::GetCastTypeName(CurrentSpellInfo.CastType);
		sss.Append(" + ");
		FString aa = UHelperFunctions::GetElementName(CurrentSpellInfo.Element);
		sss.Append(aa);
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Failed to find Niagara System %s"), *sss));
		SET_WARN_COLOR(COLOR_RED);
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Niagara System %s"), *sss);
		CLEAR_WARN_COLOR();
		return nullptr;
	}
	return nullptr;
}

void UBaseSpellManager::SpawnHitParticle(FVector Location)
{
	UNiagaraSystem* System = GetNiagaraSystem(CurrentSpellInfo.Element, CurrentSpellInfo.CastType, SpellFXType::ON_HIT);

	
	UWorld* World = GetWorld();
	if (!World) return;

	if (System)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, System, Location, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::AutoRelease, true);
	}
	else
	{
		FString ss = "";
		ss += UHelperFunctions::GetElementName(CurrentSpellInfo.Element);
		ss += UHelperFunctions::GetCastTypeName(CurrentSpellInfo.CastType);
		ss += UHelperFunctions::GetSpellFXTypeName(SpellFXType::ON_HIT);

		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Failed to spawn Niagara System %s"), *ss));
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Niagara System %s"), *ss);
	}
}

AActor* UBaseSpellManager::GetCaster() const
{
	return Caster;
}

void UBaseSpellManager::SetVFXDataTable()
{
	ConstructorHelpers::FObjectFinder<UDataTable>DataTableAsset(TEXT("DataTable'/Game/GameSettings/DT_SpellVFX.DT_SpellVFX'"));
	UDataTable* DT = DataTableAsset.Object;
	if (DT)
	{
		VFX_DataTable = DT;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Yellow, TEXT("No VFX DataTable found!"));
	}
}

