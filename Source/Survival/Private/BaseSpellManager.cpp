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
#include "MultithreadCalculator.h"

// Sets default values for this component's properties
ABaseSpellManager::ABaseSpellManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//VFXComponent = CreateDefaultSubobject<USpellVFXComponent>(FName(TEXT("VFXComponent")));
	ISMComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(TEXT("ISMComp")));
	RootComponent = ISMComp;
	ISMComp->SetGenerateOverlapEvents(false);
	ISMComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ISMComp->SetCollisionProfileName(FName(TEXT("NoCollision")));
	ISMComp->SetCanEverAffectNavigation(false);
	ISMComp->UpdateBounds();

	SpellPoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("SpellPoolManager")));

	MultithreadCalculatorComponent = CreateDefaultSubobject<UMultithreadCalculator>(FName(TEXT("MultithreadCalculatorComponent")));
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

	MoveSpells();
	CheckForCollisions();
	// ...

}

void ABaseSpellManager::CastSpellLoop()
{
	FSpellRuntimeInfo Info;

	if (CurrentSpellInfo.CastType == CastType::PROJECTILE || CurrentSpellInfo.CastType == CastType::STORM)
	{
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

void ABaseSpellManager::CastSpell(FSpellRuntimeInfo SpellRuntimeInfo)
{
	if (!Caster)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Caster is nullptr, so cannot cast spell"));
		return;
	}
	UWorld* World = GetWorld();
	if (!World) return;

	const int InstanceIndex = GetAvailableSpellInstanceIndex();

	SpellInstances[InstanceIndex].OrderIndex = SpellRuntimeInfo.OrderIndex;
	SpellInstances[InstanceIndex].IsActive = true;
	SpellInstances[InstanceIndex].Transform.SetLocation(GetStartingSpellLocation());
	SpellInstances[InstanceIndex].SpawnTime = World->TimeSeconds;
	//UpdateDirection(InstanceIndex);
	//SpellInstances[InstanceIndex].CurrentDirection = GetDirection(InstanceIndex);
	UpdateTarget(InstanceIndex);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("%d"), SpellInstances[InstanceIndex].CurrentDirection.X));
	ISMComp->UpdateInstanceTransform(InstanceIndex, SpellInstances[InstanceIndex].Transform, true, true, true);

}

void ABaseSpellManager::MoveSpells()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UpdateInstanceTransforms();

	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			ISMComp->UpdateInstanceTransform(i, SpellInstances[i].Transform, true, false, true);

			if (World->TimeSeconds - SpellInstances[i].SpawnTime > 9.3f)// TODO optimize Doesnt need to be called every frame
			{
				ResetInstance(i); 
			}
		}
	}

	ISMComp->MarkRenderStateDirty();
}

void ABaseSpellManager::UpdateInstanceTransforms()
{
	return;
}

FVector ABaseSpellManager::UpdateDirection(const int Index)
{
	return FVector();
}

void ABaseSpellManager::OnInstanceCollided(int Index, AActor* Actor)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (Actor)
	{
		if (Actor != Caster && Actor != this && !SpellInstances[Index].CollidedActors.Contains(Actor))
		{
			if (Actor->Implements<UCombatInterface>())
			{
				ICombatInterface::Execute_OnCollidedWithSpell(Actor, this);
			}
		}
	}
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, HitNS, SpellInstances[Index].Transform.GetLocation(), FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::AutoRelease, true);
	ResetInstance(Index);
}


void ABaseSpellManager::ResetInstance(const int Index)
{
	SpellInstances[Index].Reset();
	ISMComp->UpdateInstanceTransform(Index, SpellInstances[Index].Transform, true, true, true);
}

void ABaseSpellManager::CheckForCollisions()
{
	// TODO check how much time this takes
	if (!Caster) return;
	ICombatInterface* TempInterface = Cast<ICombatInterface>(Caster);
	TArray<AActor*> ActorsToCheck = TempInterface->GetAliveEnemies();

	/*
	if (MultithreadCalculatorComponent && ActorsToCheck.Num() > 0 && SpellInstances.Num() > 0)
	{
		TMap<int32, int32> CIndices = MultithreadCalculatorComponent->GetCalcData();

		if (CIndices.Num() > 0)
		{
			TArray<int32> ss;
			CIndices.GetKeys(ss);

			if (ss.Num() > 0)
			{
				for (int32 spellindex : ss)
				{
					int32 TempActorIndex = CIndices.FindRef(spellindex);
					if (ActorsToCheck.Num() > TempActorIndex)
					{
						if (ActorsToCheck[TempActorIndex])
						{
							OnInstanceCollided(spellindex, ActorsToCheck[TempActorIndex]);
						}
					}

				}
			}
		}
	}
	*/
	/*
	if (MultithreadCalculatorComponent && ActorsToCheck.Num() > 0 && SpellInstances.Num() > 0)
	{
		TMap<int32, int32> CIndices = MultithreadCalculatorComponent->GetCalcData();

		if (CIndices.Num() > 0)
		{
			TArray<int32> ss;
			CIndices.GetKeys(ss);

			if (ss.Num() > 0)
			{
				for (int32 spellindex : ss)
				{
					int32 TempActorIndex = CIndices.FindRef(spellindex);
					if (ActorsToCheck.Num() > TempActorIndex)
					{
						if (ActorsToCheck[TempActorIndex])
						{
							OnInstanceCollided(spellindex, ActorsToCheck[TempActorIndex]);
						}
					}

				}
			}
		}
	}
	*/


	//Single threaded
	/*
	for (int j = 0; j < SpellInstances.Num(); j++)
	{
		for (int i = 0; i < ActorsToCheck.Num(); i++)
		{
			const FVector SpellLoc = SpellInstances[j].Transform.GetLocation();
			if (SpellLoc.Z < 150.f) // Optimisation for storm type
			{
				AActor* TempActor = ActorsToCheck[i];
				if (TempActor != this && TempActor != Caster && TempActor)// && !CollidedActors.Contains(Actor) 
				{
					float Dist = FVector::Dist(SpellLoc, TempActor->GetActorLocation() + FVector(0.f, 0.f, 100.f));

					if (Dist < 70)
					{
						OnInstanceCollided(j, TempActor);
					}
				}
			}
		}
	}*/
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

	FSpellRuntimeInfo TempInfo;
	TempInfo.Reset();
	SpellInstances.Add(TempInfo);
	SpellInstances.Last().Transform.SetScale3D(FVector(1.f, 1.f, 1.f));
	SpellInstances.Last().Transform.SetLocation(GetStartingSpellLocation());
	SpellInstances.Last().IsActive = true;
	FTransform Transform;

	SpellInstances.Last().ISMIndex = ISMComp->AddInstance(SpellInstances.Last().Transform);

	return SpellInstances.Last().ISMIndex;
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

	World->GetTimerManager().SetTimer(DebugTimerHandle, this, &ABaseSpellManager::DebugValues, 0.03f, true);
	
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
	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Yellow, FString::Printf(TEXT("Spell num: %i"), SpellInstances.Num()));
	
	if (!Caster) return;
	ICombatInterface* TempInterface = Cast<ICombatInterface>(Caster);
	TArray<AActor*> ActorsToCheck = TempInterface->GetAliveEnemies();

	/*
	if (MultithreadCalculatorComponent)
	{
		TArray<FVector> SpellLocs;
		TArray<FVector> EnemyLocs;
		for (int j = 0; j < SpellInstances.Num(); j++)
		{
			SpellLocs.Add(SpellInstances[j].Transform.GetLocation());
		}
		for (int j = 0; j < ActorsToCheck.Num(); j++)
		{
			EnemyLocs.Add(ActorsToCheck[j]->GetActorLocation());
		}

		if (SpellLocs.Num() > 0 && EnemyLocs.Num() > 0)
		{
			MultithreadCalculatorComponent->InitCalculations(SpellLocs, EnemyLocs);
		}
	}
	*/

	
	for (int j = 0; j < SpellInstances.Num(); j++)
	{
		for (int i = 0; i < ActorsToCheck.Num(); i++)
		{
			const FVector SpellLoc = SpellInstances[j].Transform.GetLocation();
			if (SpellLoc.Z < 150.f) // Optimisation for storm type
			{
				AActor* TempActor = ActorsToCheck[i];
				if (TempActor != this && TempActor != Caster && TempActor)// && !CollidedActors.Contains(Actor) 
				{
					float Dist = FVector::Dist(SpellLoc, TempActor->GetActorLocation() + FVector(0.f, 0.f, 100.f));

					if (Dist < 70)
					{
						OnInstanceCollided(j, TempActor);
					}
				}
			}
		}
	}
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
	if (!Caster) return FVector();

	FVector Vec = Caster->GetActorLocation();

	if (CurrentSpellInfo.CastType == CastType::STORM)
	{
		Vec.Z += 2000.f;

		Vec += Caster->GetActorForwardVector() * 500;
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
				Mesh = SpellVFXInfo->MainMesh.LoadSynchronous();
				Mat = SpellVFXInfo->MainMaterial.LoadSynchronous();
				HitNS = SpellVFXInfo->HitFX.LoadSynchronous();

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

void ABaseSpellManager::UpdateTarget(const int Index)
{

}