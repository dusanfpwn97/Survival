// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpell.h"
#include "CombatInterface.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "BaseSpellManager.h"
#include "SpellDatatypes.h"
#include "HelperFunctions.h"
#include "SpellVFXComponent.h"

// Sets default values
ABaseSpell::ABaseSpell()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetupComponents();
}

void ABaseSpell::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Move();
}

void ABaseSpell::CheckTarget()
{
	if (SpellManager) { if (SpellManager->GetIsStaticLocationSpell()) return; }


	UWorld* World = GetWorld();
	if (!World) return;
	if (TargetActor)
	{
		if (TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			if (!ICombatInterface::Execute_GetIsAlive(TargetActor) && !FinishTimerHandle.IsValid())
			{
				World->GetTimerManager().SetTimer(FinishTimerHandle, this, &ABaseSpell::Finish, 8.f, false);
				//World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
				TargetActor = nullptr; // TODO look into how to optimize. Maybe some timers are called during these 8 seconds
			}
		}
	}
	else if(!FinishTimerHandle.IsValid()) //TODO
	{
		World->GetTimerManager().SetTimer(FinishTimerHandle, this, &ABaseSpell::Finish, 8.f, false);
		//World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
	}

}

void ABaseSpell::Move()
{
	/*
	if ((SpellManager->CurrentSpellInfo.CastType == CastType ::NOVA || SpellManager->CurrentSpellInfo.CastType == CastType::SHIELD) && SpellManager->Caster)
	{
		SetActorLocation(SpellManager->Caster->GetActorLocation());
		return;
	}
	*/
	// If target is not valid, z = 0 so that spell keeps going on without hitting the ground or going in the sky
	/*
	if (SpellManager->CurrentSpellInfo.CastType != CastType::STORM)
	{
		if (TargetActor)
		{
			if (TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
			{
				if (CurrentDirection.Z < 0 && !ICombatInterface::Execute_GetIsAlive(TargetActor))
				{
					CurrentDirection.Z = 0;
				}
			}
		}
		else CurrentDirection.Z = 0;
	}
	
	*/
	
}

void ABaseSpell::Finish()
{
	//SetActorTickEnabled(false);
	ClearAllTimers();
	UWorld* World = GetWorld();

	if (!World)
	{
		Destroy();
	}
	else
	{
		World->GetTimerManager().SetTimer(ResetTimerHandle, this, &ABaseSpell::Reset_Implementation, 1.f, true);
	}

	TargetActor = nullptr;
	RemoveCollision();
	VFXComponent->StopMainVFX();

	SpellManager->OnSpellFinished(this);
}

void ABaseSpell::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp == BaseCollider && OtherActor != SpellManager->GetCaster() && OtherActor != this && !CollidedActors.Contains(OtherActor))
	{
		if (OtherActor->Implements<UCombatInterface>())
		{
			ICombatInterface::Execute_OnCollidedWithSpell(OtherActor, this);
		}

		CollidedActors.Add(OtherActor);

		if (VFXComponent) VFXComponent->StartHitVFX();
		Finish();
	}
}

void ABaseSpell::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ABaseSpell::Start_Implementation()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ClearAllTimers();
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	World->GetTimerManager().SetTimer(SetupCollisionTimerHandle, this, &ABaseSpell::SetupCollision, 0.1f, true);
	HasDeterminedDirection = false;
	VFXComponent->StartMainVFX();
	CollidedActors.Empty();

	SetWatchdogTimers();

	if (SpellManager)
	{
		if (SpellManager->Caster)
		{
			//Set initial direction so that spells can move if there is no inital target
			CurrentDirection = SpellManager->Caster->GetActorForwardVector();
		}

		if (!SpellManager->GetIsStaticLocationSpell()) CurrentDirection = GetMoveDirection();
	}

}

void ABaseSpell::Reset_Implementation()
{
	SetActorTickEnabled(false);
	Finish();
	ClearAllTimers();

	SetActorHiddenInGame(true);
	CollidedActors.Empty();

	VFXComponent->Hibernate();
	UWorld* World = GetWorld();
	if (!World) return;

	if (!IsMarkedForDestruction)
	{
		if (CurrentPoolManager)
		{
			IPoolInterface::Execute_ReleaseToPool(CurrentPoolManager, this);
		}
		else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("CurrentPoolManager null BaseSpell::ResetImplementation!")));
	}
	else
	{
		Destroy();
	}
}

void ABaseSpell::SetTarget_Implementation(AActor* NewTarget)
{
	TargetActor = NewTarget;
	UWorld* World = GetWorld();
	if (!World) return;

	//World->GetTimerManager().SetTimer(UpdateDirectionTimerHandle, this, &ABaseSpell::UpdateMoveDirection, 0.05f, true);
	World->GetTimerManager().SetTimer(CheckTargetTimerHandle, this, &ABaseSpell::CheckTarget, 0.25f, true);
	CheckTarget();
	CurrentDirection = GetMoveDirection();
}

void ABaseSpell::SetSpellManager_Implementation(UBaseSpellManager* NewSpellManager)
{
	SpellManager = NewSpellManager;
	if (SpellManager)
	{
		VFXComponent->SetupVFX(SpellManager, this);
		SetWatchdogTimers();
		BaseCollider->SetSphereRadius(SpellManager->CurrentSpellInfo.Radius);
		
	}
}

void ABaseSpell::SetSpawner_Implementation(UObject* Object)
{
	if (!Object)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spawner not valid! Should not happen. BaseEnemy.cpp -> SetSpawner_Implementation"));
		return;
	}
	CurrentPoolManager = Object;
}

void ABaseSpell::SetupComponents()
{
	BaseCollider = CreateDefaultSubobject<USphereComponent>(FName(TEXT("BaseCollider")));
	RootComponent = BaseCollider;

	VFXComponent = CreateDefaultSubobject<USpellVFXComponent>(FName(TEXT("VFXComponent")));

	BaseCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseSpell::OnOverlapBegin);
	BaseCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseSpell::OnOverlapEnd);

	SetupCollision();
}

void ABaseSpell::RemoveCollision()
{
	BaseCollider->SetGenerateOverlapEvents(false);
	BaseCollider->SetCollisionProfileName(FName(TEXT("NoCollision")));
}

void ABaseSpell::SetupCollision()
{

	BaseCollider->SetCollisionProfileName(FName(TEXT("Spell")));
	BaseCollider->SetGenerateOverlapEvents(true);
}

void ABaseSpell::ClearAllTimers()
{
	UWorld* World = GetWorld();
	{
		World->GetTimerManager().ClearTimer(ResetTimerHandle);
		World->GetTimerManager().ClearTimer(FinishTimerHandle);
		World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
		World->GetTimerManager().ClearTimer(UpdateDirectionTimerHandle);
		World->GetTimerManager().ClearTimer(SetupCollisionTimerHandle);
		
	}
}

UBaseSpellManager* ABaseSpell::GetSpellManager() const
{
	return SpellManager;
}

void ABaseSpell::SetWatchdogTimers()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		IsMarkedForDestruction = true;
		Reset_Implementation();
		return;
	}

	World->GetTimerManager().SetTimer(ResetTimerHandle, this, &ABaseSpell::Reset_Implementation, WatchdogTime, false);
}

FVector ABaseSpell::GetMoveDirection()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("Called base GetMoveDirection Function in BaseSpell. Shoud be overriden in children. Investigate..."));
	return CurrentDirection;
}