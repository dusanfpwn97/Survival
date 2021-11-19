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
#include "SpellFunctionLibrary.h"
#include "SpellProjectile.h"
#include "SpellFlick.h"
#include "SpellStorm.h"
#include "SpellShield.h"
#include "SpellVFXComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values for this component's properties
UBaseSpellManager::UBaseSpellManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	SpellPoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("SpellPoolManager")));
	//VFXComponent = CreateDefaultSubobject<USpellVFXComponent>(FName(TEXT("VFXComponent")));
	ISMComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(TEXT("ISMComp")));
	SetVFXDataTable();
}

// Called when the game starts
void UBaseSpellManager::BeginPlay()
{
	Super::BeginPlay();

}

void UBaseSpellManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//UpdateSpellLocations();
	// ...
	if (Caster)
	{
		FAttachmentTransformRules Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;

		ISMComp->AttachToComponent(Caster->GetRootComponent(), Rules);
	}

}

void UBaseSpellManager::CastSpellLoop()
{
	FSpellRuntimeInfo Info;

	if (CurrentSpellInfo.CastType == CastType::PROJECTILE)
	{
		//Info.StartingLocation.Z += -50;
		if(SpellModifiers.Contains(SpellModifier::SPLIT))
		{
			if (Caster)
			{
				for (int i = 0; i <= 8; i++)
				{
					Info.OrderIndex = i;
					
					CastSpell(Info);
				}
				return;
			}
		}
	}

	CastSpell(Info);
}

void UBaseSpellManager::CastSpell(FSpellRuntimeInfo SpellRuntimeInfo)
{
	if (!Caster)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Caster is nullptr, so cannot cast spell"));
		return;
	}

	int InstanceIndex = GetAvailableSpellInstanceIndex();
	
	FSpellRuntimeInfo NewInfo = SpellInstances[InstanceIndex];
	NewInfo.OrderIndex = SpellRuntimeInfo.OrderIndex;
	NewInfo.IsActive = true;
	NewInfo.Transform.SetLocation(Caster->GetActorLocation());

	ISMComp->UpdateInstanceTransform(InstanceIndex, NewInfo.Transform, true, true, true);

	
}

void UBaseSpellManager::UpdateSpellLocations()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			bool IsDirty = false;
			FTransform NewTransform = SpellInstances[i].Transform;
			FVector NewLoc = NewTransform.GetLocation();
			NewLoc += SpellInstances[i].CurrentDirection * CurrentSpellInfo.Speed * World->DeltaTimeSeconds;

			if (i == SpellInstances.Num() - 1) IsDirty = true;
			ISMComp->UpdateInstanceTransform(i, NewTransform, true, IsDirty, true);
		}
	}
}

UStaticMesh* UBaseSpellManager::GetMeshFromDT()
{
	if (!VFX_DataTable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Couldn't find Datatable for Spell VFX")));
		return nullptr;
	}

	UStaticMesh* Mesh = nullptr;

	for (auto it : VFX_DataTable->GetRowMap())
	{
		FSpellVFXInfo* SpellVFXInfo = (FSpellVFXInfo*)(it.Value);
		//FSpellVFXInfo* SpellVFXInfo = Cast<FSpellVFXInfo>(it.Value);
		if (SpellVFXInfo)
		{
			if (SpellVFXInfo->Binding.Element == CurrentSpellInfo.Element && SpellVFXInfo->Binding.CastType == CurrentSpellInfo.CastType)
			{
				Mesh = SpellVFXInfo->MainMesh.LoadSynchronous();
				return Mesh;
			}
		}
	}
	return Mesh;
	/*
	UNiagaraSystem* NS = nullptr;

	for (auto it : VFX_DataTable->GetRowMap())
	{

		FSpellVFXInfo* SpellVFXInfo = (FSpellVFXInfo*)(it.Value);
		//FSpellVFXInfo* SpellVFXInfo = Cast<FSpellVFXInfo>(it.Value);
		if (SpellVFXInfo)
		{
			if (SpellVFXInfo->Binding.Element == SpellManagerOwner->CurrentSpellInfo.Element && SpellVFXInfo->Binding.CastType == SpellManagerOwner->CurrentSpellInfo.CastType)
			{
				if (SpellFXType == SpellFXType::ON_SPAWN)
				{
					NS = SpellVFXInfo->SpawnFX.LoadSynchronous();
					return NS;
				}
				else if (SpellFXType == SpellFXType::MAIN)
				{
					NS = SpellVFXInfo->MainFX.LoadSynchronous();
					return NS;
				}
				else if (SpellFXType == SpellFXType::ON_HIT)
				{
					NS = SpellVFXInfo->HitFX.LoadSynchronous();
					return NS;
				}
			}
		}
	}*/
}

int UBaseSpellManager::GetAvailableSpellInstanceIndex()
{
	if (SpellInstances.Num() > 0)
	{
		for (int i = 0; i < SpellInstances.Num(); i++)
		{
			if (!SpellInstances[i].IsActive)
			{
			//	return i;
			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("%i"), 33));

	FSpellRuntimeInfo Info;

	// Not found. Add new

	SpellInstances.Add(Info);
	FTransform Transform;
	Transform.SetScale3D(FVector(1, 1, 1));
	Transform.SetLocation(GetStartingSpellLocation());

	
	Info.ISMIndex = ISMComp->AddInstance(Transform);
	Info.CurrentDirection = FVector(1, 0, 0);
	Info.IsActive = true;
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("%d"), Transform.GetLocation().X));
	return Info.ISMIndex;
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

void UBaseSpellManager::InitSpellManager(FSpellInfo NewSpellInfo)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo = NewSpellInfo;
	UpdateIsTargetlessSpell();
	UWorld* World = GetWorld();
	if (!World) return;
	World->GetTimerManager().ClearTimer(MainSpellCastTimerHandle);
	
	StartCastSpellTimer(!IsSingleCastSpell());
	AddSpellModifier(SpellModifier::SPLIT);
	ISMComp->SetStaticMesh(GetMeshFromDT());

	//CastSpell(); // Debug
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
	SpellPoolManager->AllSpawnedActors.Empty();
}

void UBaseSpellManager::DestroySpellManager()
{
	MarkAllSpellsForDestruction();
}

void UBaseSpellManager::UpdateSpellModifier(SpellModifier NewSpellModifier)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo.SpellModifier = NewSpellModifier;
}

FVector UBaseSpellManager::GetStartingSpellLocation()
{
	//FVector Vec = ICombatInterface::Execute_GetSpellCastLocation(Caster);
	FVector Vec = Caster->GetActorLocation();
	Vec.Z += 100.f;
	if (CurrentSpellInfo.CastType == CastType::STORM)
	{
		Vec.Z += 1000.f;
		Vec.X += FMath::FRandRange(-600.f, 600.f);
		Vec.Y += FMath::FRandRange(-600.f, 600.f);
	}
	
	return Vec;
}

void UBaseSpellManager::AddSpellModifier(SpellModifier NewSpellModifier)
{
	if(USpellFunctionLibrary::IsSpellModifierCompatible(NewSpellModifier, CurrentSpellInfo.CastType) && NewSpellModifier != SpellModifier::NO_MODIFIER)
	{
		SpellModifiers.Add(NewSpellModifier);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("%s spell modifier is not compatible with %s cast type"), *USpellFunctionLibrary::GetSpellModifierName(NewSpellModifier), *USpellFunctionLibrary::GetCastTypeName(CurrentSpellInfo.CastType)));
	}
}

void UBaseSpellManager::RemoveSpellModifier(SpellModifier NewSpellModifier)
{
	SpellModifiers.Remove(NewSpellModifier);
}

void UBaseSpellManager::StartCastSpellTimer(bool ShouldLoop)
{
	GetWorld()->GetTimerManager().SetTimer(MainSpellCastTimerHandle, this, &UBaseSpellManager::CastSpellLoop, CurrentSpellInfo.Cooldown, ShouldLoop);
}
 
void UBaseSpellManager::OnSpellFinished(ABaseSpell* FinishedSpell)
{
	if (IsSingleCastSpell())
	{
		StartCastSpellTimer(false);
	}
}

bool UBaseSpellManager::IsSingleCastSpell()
{
	if (CurrentSpellInfo.CastType == CastType::NOVA || CurrentSpellInfo.CastType == CastType::SHIELD) return true;
	else return false;
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

AActor* UBaseSpellManager::GetCaster() const
{
	return Caster;
}

bool UBaseSpellManager::GetIsStaticLocationSpell() const
{
	return IsTargetlessSpell;
}

void UBaseSpellManager::UpdateIsTargetlessSpell()
{
	IsTargetlessSpell = false;
	TArray<CastType> Temp = USpellFunctionLibrary::GetAllTargetlessCastTypes();
	if (Temp.Contains(CurrentSpellInfo.CastType))
	{
		IsTargetlessSpell = true;
	}
}

UClass* UBaseSpellManager::GetSpellClassForSpawning()
{
	if (CurrentSpellInfo.CastType == CastType::PROJECTILE) return ASpellProjectile::StaticClass(); 
	if (CurrentSpellInfo.CastType == CastType::FLICK) return ASpellFlick::StaticClass();
	if (CurrentSpellInfo.CastType == CastType::STORM) return ASpellStorm::StaticClass();
	if (CurrentSpellInfo.CastType == CastType::SHIELD) return ASpellShield::StaticClass();

	
	return nullptr;
}

void UBaseSpellManager::UpdateCastType(CastType NewCastType)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo.CastType = NewCastType;
	UpdateIsTargetlessSpell();
}


/*
UClass* ss = GetSpellClassForSpawning();
if (!ss)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Spell couldn't be spawned Spell class us null. Need to set it in GetSpellClassForSpawning(). Shouldn't happen! UBaseSpellManager::CastSpell"));
	return;
}


//bool IsCached;
//AActor* SpellToCast = SpellPoolManager->GetAvailableActor(ss, IsCached);

if (!SpellToCast)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Spell couldn't be spawned. Shouldn't happen! UBaseSpellManager::CastSpell"));
	return;
}


ICombatInterface::Execute_SetSpellManager(SpellToCast, this);
IPoolInterface::Execute_SetOrderIndex(SpellToCast, AdditonalInfo.OrderIndex);
SpellToCast->SetActorLocation(AdditonalInfo.StartingLocation);
*/