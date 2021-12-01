// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpellManager.h"
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
#include "SpellVFXComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
//#include "MultithreadCalculator.h"

// Sets default values for this component's properties
ABaseSpellManager::ABaseSpellManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	VFXComponent = CreateDefaultSubobject<USpellVFXComponent>(FName(TEXT("VFXComponent")));
	ISMComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(TEXT("ISMComp")));
	RootComponent = ISMComp;
	ISMComp->SetGenerateOverlapEvents(false);
	ISMComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ISMComp->SetCollisionProfileName(FName(TEXT("NoCollision")));
	ISMComp->SetCanEverAffectNavigation(false);
	ISMComp->UpdateBounds();

	//SpellPoolManager = CreateDefaultSubobject<UPoolManager>(FName(TEXT("SpellPoolManager")));

	//MultithreadCalculatorComponent = CreateDefaultSubobject<UMultithreadCalculator>(FName(TEXT("MultithreadCalculatorComponent")));

}

// Called when the game starts
void ABaseSpellManager::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseSpellManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ABaseSpellManager::CastSpell(FSpellRuntimeInfo InitialSpellRuntimeInfo)
{
	if (!Caster)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Caster is nullptr, so cannot cast spell"));
		return;
	}
	UWorld* World = GetWorld();
	if (!World) return;

	const int InstanceIndex = GetAvailableSpellInstanceIndex();

	SpellInstances[InstanceIndex] = InitialSpellRuntimeInfo;

	SpellInstances[InstanceIndex].IsActive = true;
	SpellInstances[InstanceIndex].Transform.SetLocation(GetStartingSpellLocation());
	SpellInstances[InstanceIndex].SpawnTime = World->TimeSeconds;

	if (CurrentSpellInfo.CastType != CastType::NOVA)
	{
		SpellInstances[InstanceIndex].CurrentRadius = CurrentSpellInfo.Radius;
	}
	else
	{
		SpellInstances[InstanceIndex].CurrentRadius = 10.f;
		SpellInstances[InstanceIndex].Transform.SetScale3D(FVector(0.1f, 0.1f, 0.1f));
	}

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
		}
	}

	ISMComp->MarkRenderStateDirty();
}

void ABaseSpellManager::UpdateInstanceTransforms()
{
	return;
}

void ABaseSpellManager::UpdateDirection(const int Index)
{
}

void ABaseSpellManager::CollideInstance(int Index, AActor* Actor)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (SpellInstances[Index].CollidedActors.Num() > 0 && CurrentSpellInfo.CastType != CastType::NOVA) return;

	if (Actor)
	{
		if (Actor != Caster && Actor != this && !SpellInstances[Index].CollidedActors.Contains(Actor))
		{
			if (Actor->Implements<UCombatInterface>())
			{
				ICombatInterface* TempInterface = Cast<ICombatInterface>(Actor);
				TempInterface->OnCollidedWithSpell(this, SpellModifier::NO_MODIFIER);

				SpellInstances[Index].CollidedActors.Add(Actor);

			}
		}
	}

	if (SpellModifiers.Contains(SpellModifier::EXPLODE_ON_IMPACT))
	{
		CreateExplosion(SpellInstances[Index].Transform.GetLocation(), 200.f);
	}
	
	VFXComponent->SpawnHitVFX(SpellInstances[Index].Transform.GetLocation());

	if (CurrentSpellInfo.CastType != CastType::NOVA)
	{
		ResetInstance(Index);
	}

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
	ICombatInterface* CasterInterface = Cast<ICombatInterface>(Caster);
	TArray<AActor*> ActorsToCheck = CasterInterface->GetAliveEnemies();

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

					if (Dist < CurrentSpellInfo.Radius && TempActor)
					{
						ICombatInterface* Temp = Cast<ICombatInterface>(TempActor);
						if (Temp)
						{
							if (Temp->GetIsAlive())
							{
								CollideInstance(j, TempActor);
								OnSpellFinished(j);
								goto EndCurrentLoop;
							}
						}

					}
				}
			}
		}

		EndCurrentLoop: ;
	}
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

void ABaseSpellManager::InitSpellManager(FSpellInfo NewSpellInfo)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo = NewSpellInfo;

	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(MainSpellCastTimerHandle);
	World->GetTimerManager().SetTimer(DebugTimerHandle, this, &ABaseSpellManager::DebugValues, 0.03f, true);
	// Set random watchdog update time to offset potential syncing of a lot of spells (performance improvement because lot of for loops).
	float TempWatchdogUpdateTime = FMath::FRandRange(0.2f, 0.3f); // TODO low priority: find optimal time for a lot of spells
	
	World->GetTimerManager().SetTimer(WatchdogTimer, this, &ABaseSpellManager::SpellLifetimeCheck, TempWatchdogUpdateTime, true);
	
	StartCastSpellTimer(!IsSingleCastSpell());
	AddSpellModifier(SpellModifier::EXPLODE_ON_IMPACT);
	UStaticMesh* Mesh = nullptr;
	UMaterialInterface* Mat = nullptr;

	VFXComponent->SetupVFX(this);
	VFXComponent->GetVFXDataFromDT(Mesh, Mat);


	ISMComp->SetStaticMesh(Mesh);
	ISMComp->SetMaterial(0, Mat);
	//CastSpell(); // Debug
}

void ABaseSpellManager::DebugValues()
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Yellow, FString::Printf(TEXT("Spell num: %i"), SpellInstances.Num()));
	/*
	if (!Caster) return;
	ICombatInterface* TempInterface = Cast<ICombatInterface>(Caster);
	TArray<AActor*> ActorsToCheck = TempInterface->GetAliveEnemies();

	
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


}

void ABaseSpellManager::MarkAllSpellsForDestruction()
{
	for (FSpellRuntimeInfo SpellInstance : SpellInstances)
	{
		SpellInstance.Reset();
	}
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

	if (CurrentSpellInfo.CastType == CastType::NOVA)
	{
		Vec.Z = 50.f;
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
 
void ABaseSpellManager::OnSpellFinished(const int32 Index)
{
	if (IsSingleCastSpell())
	{
		StartCastSpellTimer(false);
	}
}

bool ABaseSpellManager::IsSingleCastSpell()
{
	if (CurrentSpellInfo.CastType == CastType::SHIELD) return true;
	else return false;
}

void ABaseSpellManager::UpdateCastType(CastType NewCastType)
{
	MarkAllSpellsForDestruction();
	CurrentSpellInfo.CastType = NewCastType;
}

AActor* ABaseSpellManager::GetAppropriateTarget(const int32 Index, const int32 OrderIndex)
{
	if (!Caster) return nullptr;
	if (!Caster->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) return nullptr;
	ICombatInterface* TempInterface = Cast<ICombatInterface>(Caster);
	if (!TempInterface) return nullptr;

	if (CurrentSpellInfo.TargetMode == TargetMode::CLOSEST)
	{
		if (OrderIndex == 0) return TempInterface->GetClosestEnemy();
	}

	return nullptr;

	/*
	UWorld* World = GetWorld();
	if (!World) return;

	//World->GetTimerManager().SetTimer(UpdateDirectionTimerHandle, this, &ABaseSpell::UpdateMoveDirection, 0.05f, true);
	World->GetTimerManager().SetTimer(CheckTargetTimerHandle, this, &AFlickSpellManager::CheckTarget, 0.25f, true);
	CheckTarget();*/
}

TArray<AActor*> ABaseSpellManager::GetAppropriateTargets(const int32 NumOfTargets)
{
	if (!Caster) return TArray<AActor*>();
	if (!Caster->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) return TArray<AActor*>();
	ICombatInterface* TempInterface = Cast<ICombatInterface>(Caster);
	if (!TempInterface) return TArray<AActor*>();

	if (CurrentSpellInfo.TargetMode == TargetMode::CLOSEST)
	{
		return TempInterface->GetClosestEnemies(NumOfTargets);
	}

	return TArray<AActor*>();
}

void ABaseSpellManager::SpellLifetimeCheck()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			if (World->TimeSeconds - SpellInstances[i].SpawnTime > SpellLifetime)
			{
				if (Caster && CurrentSpellInfo.CastType != CastType::NOVA) // TODO if spells are cast by someone other than player, it will dissapear when it's not desired
				{
					// if the spell is far away reset it (optimisation)
					if (FVector::Dist(SpellInstances[i].Transform.GetLocation(), Caster->GetActorLocation()) > 3000.f)
					{
						ResetInstance(i);
					}
				}
				else
				{
					ResetInstance(i);
				}
			}
		}
	}
}

void ABaseSpellManager::CreateExplosion(FVector Location, float Radius)
{
	if (!Caster) return;

	ICombatInterface* CasterInterface = Cast<ICombatInterface>(Caster);
	TArray<AActor*> ActorsToCheck = CasterInterface->GetAliveEnemies();

	// Damage
	for (int i = 0; i < ActorsToCheck.Num(); i++)
	{
		AActor* TempActor = ActorsToCheck[i];
		if (TempActor != this && TempActor != Caster && TempActor)
		{
			float Dist = FVector::Dist(Location, TempActor->GetActorLocation() + FVector(0.f, 0.f, 100.f));

			if (Dist < 230.f)
			{
				if (TempActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
				{
					ICombatInterface* Temp = Cast<ICombatInterface>(TempActor);
					if (Temp)
					{
						if (Temp->GetIsAlive())
						{
							Temp->OnCollidedWithSpell(this, SpellModifier::EXPLODE_ON_IMPACT);

						}
					}
				}
			}
		}
	}
}
