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
#include "Materials/MaterialInterface.h"

// Sets default values for this component's properties
ABaseSpellManager::ABaseSpellManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpellPoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("SpellPoolManager")));
	//VFXComponent = CreateDefaultSubobject<USpellVFXComponent>(FName(TEXT("VFXComponent")));
	ISMComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(TEXT("ISMComp")));
	RootComponent = ISMComp;
	ISMComp->SetGenerateOverlapEvents(false);
	ISMComp->SetCollisionProfileName(FName(TEXT("NoCollision")));
	ISMComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ISMComp->SetCanEverAffectNavigation(false);

	SetVFXDataTable();

}

// Called when the game starts
void ABaseSpellManager::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseSpellManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpellLocations();
	// ...

}

void ABaseSpellManager::CastSpellLoop()
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
				//return;
			}
		}
	}

	CastSpell(Info);
}

void ABaseSpellManager::CastSpell(FSpellRuntimeInfo SpellRuntimeInfo)
{
	if (!Caster)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Caster is nullptr, so cannot cast spell"));
		return;
	}
	UWorld* World = GetWorld();
	if (!World) return;

	int InstanceIndex = GetAvailableSpellInstanceIndex();

	SpellInstances[InstanceIndex].OrderIndex = SpellRuntimeInfo.OrderIndex;
	SpellInstances[InstanceIndex].IsActive = true;
	SpellInstances[InstanceIndex].Transform.SetLocation(Caster->GetActorLocation());
	SpellInstances[InstanceIndex].SpawnTime = World->TimeSeconds;

	ISMComp->UpdateInstanceTransform(InstanceIndex, SpellInstances[InstanceIndex].Transform, true, true, true);

	
}

void ABaseSpellManager::UpdateSpellLocations()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ICombatInterface* TempInterface = Cast<ICombatInterface>(Caster);
	TArray<AActor*> ActorsToCheck = TempInterface->GetAliveEnemies();

	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			//
			bool IsDirty = false;
			FTransform NewTransform = SpellInstances[i].Transform;
			FVector NewLoc = NewTransform.GetLocation();
			SpellInstances[i].Velocity += (SpellInstances[i].CurrentDirection * CurrentSpellInfo.Speed*0.01) * World->DeltaTimeSeconds;

			NewLoc += SpellInstances[i].Velocity;

			SpellInstances[i].Transform.SetLocation(NewLoc);

			if (i == SpellInstances.Num() - 1) IsDirty = true;

			ISMComp->UpdateInstanceTransform(i, NewTransform, true, false, true);
			
			//

			AActor* CollidedActor = IsColliding(i, ActorsToCheck);
			if (CollidedActor)
			{
				OnInstanceCollided(i, CollidedActor);
			}

			//

			if (World->TimeSeconds - SpellInstances[i].SpawnTime > 9.3f)
			{
				ResetInstance(i);
			}
		}
	}
	ISMComp->MarkRenderStateDirty();
}

void ABaseSpellManager::OnInstanceCollided(int Index, AActor* Actor)
{
	if (Actor != Caster && Actor != this && !SpellInstances[Index].CollidedActors.Contains(Actor))
	{
		if (Actor->Implements<UCombatInterface>())
		{
			ICombatInterface::Execute_OnCollidedWithSpell(Actor, this);
		}

		//CollidedActors.Add(OtherActor);

		ResetInstance(Index);
	}
}

void ABaseSpellManager::ResetInstance(const int Index)
{
	SpellInstances[Index].Reset();
	ISMComp->UpdateInstanceTransform(Index, SpellInstances[Index].Transform, true, true, true);
}

AActor* ABaseSpellManager::IsColliding(const int Index, TArray<AActor*> &ActorsToCheck)
{
	if (!Caster) return nullptr;

	for (AActor* Actor : ActorsToCheck)
	{
		if (Actor != this/* && !CollidedActors.Contains(Actor) */&& Actor != Caster && Actor)
		{
			float Dist = FVector::Dist(SpellInstances[Index].Transform.GetLocation(), Actor->GetActorLocation());

			if (Dist < 100)
			{
				return Actor;
			}
		}
	}
	return nullptr;
}

int ABaseSpellManager::GetAvailableSpellInstanceIndex()
{
	if (SpellInstances.Num() > 0)
	{
		for (int i = 0; i < SpellInstances.Num(); i++)
		{
			if (!SpellInstances[i].IsActive)
			{
				return i;
			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("%i"), 33));

	FSpellRuntimeInfo Info;

	Info.Transform.SetScale3D(FVector(0.3, 0.3, 0.3));
	Info.Transform.SetLocation(GetStartingSpellLocation());
	Info.CurrentDirection = FVector(FMath::FRandRange(-1,1), FMath::FRandRange(-1, 1), 0);
	//Info.CurrentDirection = FVector(1,0, 0);
	Info.CurrentDirection.Normalize();
	Info.IsActive = true;

	Info.ISMIndex = ISMComp->AddInstance(Info.Transform);
	SpellInstances.Add(Info);
	return Info.ISMIndex;
}

void ABaseSpellManager::SetVFXDataTable()
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

void ABaseSpellManager::InitSpellManager(FSpellInfo NewSpellInfo)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo = NewSpellInfo;
	UpdateIsTargetlessSpell();
	UWorld* World = GetWorld();
	if (!World) return;
	World->GetTimerManager().ClearTimer(MainSpellCastTimerHandle);

	World->GetTimerManager().SetTimer(DebugTimerHandle, this, &ABaseSpellManager::DebugValues, 1.f, true);
	
	StartCastSpellTimer(!IsSingleCastSpell());
	AddSpellModifier(SpellModifier::SPLIT);
	UStaticMesh* Mesh = nullptr;
	UMaterialInterface* Mat = nullptr;
	GetVFXDataFromDT(Mesh, Mat);

	ISMComp->SetStaticMesh(Mesh);
	ISMComp->SetMaterial(0, Mat);
	//CastSpell(); // Debug
}

void ABaseSpellManager::DebugValues()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Spell num: %i"), SpellInstances.Num()));
}

void ABaseSpellManager::MarkAllSpellsForDestruction()
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

void ABaseSpellManager::DestroySpellManager()
{
	MarkAllSpellsForDestruction();
}

void ABaseSpellManager::UpdateSpellModifier(SpellModifier NewSpellModifier)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo.SpellModifier = NewSpellModifier;
}

FVector ABaseSpellManager::GetStartingSpellLocation()
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

void ABaseSpellManager::AddSpellModifier(SpellModifier NewSpellModifier)
{
	if(USpellFunctionLibrary::IsSpellModifierCompatible(NewSpellModifier, CurrentSpellInfo.CastType) && NewSpellModifier != SpellModifier::NO_MODIFIER)
		
	{
		SpellModifiers.Add(NewSpellModifier);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("%s spell modifier is not compatible with %s cast type"), *USpellFunctionLibrary::GetSpellModifierName(NewSpellModifier), *USpellFunctionLibrary::GetCastTypeName(CurrentSpellInfo.CastType)));
	}
}

void ABaseSpellManager::RemoveSpellModifier(SpellModifier NewSpellModifier)
{
	SpellModifiers.Remove(NewSpellModifier);
}

void ABaseSpellManager::StartCastSpellTimer(bool ShouldLoop)
{
	GetWorld()->GetTimerManager().SetTimer(MainSpellCastTimerHandle, this, &ABaseSpellManager::CastSpellLoop, CurrentSpellInfo.Cooldown, ShouldLoop);
}
 
void ABaseSpellManager::OnSpellFinished(ABaseSpell* FinishedSpell)
{
	if (IsSingleCastSpell())
	{
		StartCastSpellTimer(false);
	}
}

bool ABaseSpellManager::IsSingleCastSpell()
{
	if (CurrentSpellInfo.CastType == CastType::NOVA || CurrentSpellInfo.CastType == CastType::SHIELD) return true;
	else return false;
}

void ABaseSpellManager::UpdateSpellClass()
{
	SpellClassToSpawn = CurrentSpellInfo.SpellClass.LoadSynchronous();
	if (!SpellClassToSpawn)
	{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Spell class is null! BaseSpellManager.Cpp -> CastSpell"));
			return;
	}
}

AActor* ABaseSpellManager::GetCaster() const
{
	return Caster;
}

bool ABaseSpellManager::GetIsStaticLocationSpell() const
{
	return IsTargetlessSpell;
}

void ABaseSpellManager::UpdateIsTargetlessSpell()
{
	IsTargetlessSpell = false;
	TArray<CastType> Temp = USpellFunctionLibrary::GetAllTargetlessCastTypes();
	if (Temp.Contains(CurrentSpellInfo.CastType))
	{
		IsTargetlessSpell = true;
	}
}

UClass* ABaseSpellManager::GetSpellClassForSpawning()
{
	if (CurrentSpellInfo.CastType == CastType::PROJECTILE) return ASpellProjectile::StaticClass(); 
	if (CurrentSpellInfo.CastType == CastType::FLICK) return ASpellFlick::StaticClass();
	if (CurrentSpellInfo.CastType == CastType::STORM) return ASpellStorm::StaticClass();
	if (CurrentSpellInfo.CastType == CastType::SHIELD) return ASpellShield::StaticClass();

	
	return nullptr;
}

void ABaseSpellManager::UpdateCastType(CastType NewCastType)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo.CastType = NewCastType;
	UpdateIsTargetlessSpell();
}

void ABaseSpellManager::GetVFXDataFromDT(UStaticMesh*& Mesh, UMaterialInterface*& Mat)
{
	if (!VFX_DataTable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Couldn't find Datatable for Spell VFX")));
		return;
	}

	for (auto it : VFX_DataTable->GetRowMap())
	{
		FSpellVFXInfo* SpellVFXInfo = (FSpellVFXInfo*)(it.Value);
		//FSpellVFXInfo* SpellVFXInfo = Cast<FSpellVFXInfo>(it.Value);
		if (SpellVFXInfo)
		{
			if (SpellVFXInfo->Binding.Element == CurrentSpellInfo.Element && SpellVFXInfo->Binding.CastType == CurrentSpellInfo.CastType)
			{
				UStaticMesh* TempMesh = SpellVFXInfo->MainMesh.LoadSynchronous();
				UMaterialInterface* TempMat = SpellVFXInfo->MainMaterial.LoadSynchronous();
				Mesh = TempMesh;
				Mat = TempMat;
				return;
			}
		}
	}
	return;
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