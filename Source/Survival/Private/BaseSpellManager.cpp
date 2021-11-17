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

}

void UBaseSpellManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBaseSpellManager::CastSpellLoop()
{
	FVector Location = GetStartingSpellLocation();

	if (CurrentSpellInfo.CastType == CastType::PROJECTILE)
	{
		if(SpellModifiers.Contains(SpellModifier::SPLIT))
		{
			if (Caster)
			{
				for (int i = -2; i <= 2; i++)
				{
					//FVector NewLoc = Location + Caster->GetActorRightVector() * i * 100;
					
					CastSpell(Location);
				}
				return;
			}
		}
	}

	CastSpell(Location);
}

void UBaseSpellManager::CastSpell(FVector Location)
{

	UWorld* World = GetWorld();
	if (!World) return;

	if (!Caster)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Caster is nullptr, so cannot cast spell"));
		return;
	}
	
	AActor* InitialTarget = nullptr;
	if (!IsTargetlessSpell)
	{
		InitialTarget = GetActorForTarget();
	}
	
	bool IsCached;
	UClass* ss = GetSpellClassForSpawning();
	if (!ss)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Spell couldn't be spawned Spell class us null. Need to set it in GetSpellClassForSpawning(). Shouldn't happen! UBaseSpellManager::CastSpell"));
		return;
	}

	AActor* SpellToCast = SpellPoolManager->GetAvailableActor(ss, IsCached);

	if (!SpellToCast)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Spell couldn't be spawned. Shouldn't happen! UBaseSpellManager::CastSpell"));
		return;
	}
	
	
	ICombatInterface::Execute_SetSpellManager(SpellToCast, this);
	if (InitialTarget)
	{
		ICombatInterface::Execute_SetTarget(SpellToCast, InitialTarget);
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
	UpdateIsTargetlessSpell();
	UWorld* World = GetWorld();
	if (!World) return;
	World->GetTimerManager().ClearTimer(MainSpellCastTimerHandle);
	
	StartCastSpellTimer(!IsSingleCastSpell());
	AddSpellModifier(SpellModifier::SPLIT);


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
