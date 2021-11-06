// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpell.h"
#include "CombatInterface.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "BaseSpellManager.h"
#include "SpellMovementComponent.h"
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
	if (SpellMovementComponent) SpellMovementComponent->SpellOwner = this;
	Super::BeginPlay();

}

void ABaseSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Move();
}

void ABaseSpell::CheckTarget()
{
	if (SpellManager) { if (SpellManager->GetIsTargetlessSpell()) return; }


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

	//UpdateMoveDirection(); // TODO: Optimize. Current problem is thjat at the start there is visible delay
	UWorld* World = GetWorld();
	if (!World || !SpellManager) return;

	if ((SpellManager->CurrentSpellInfo.CastType == CastType ::NOVA || SpellManager->CurrentSpellInfo.CastType == CastType::SHIELD) && SpellManager->Caster)
	{
		SetActorLocation(SpellManager->Caster->GetActorLocation());
		return;
	}
	// If target is not valid, z = 0 so that spell keeps going on without hitting the ground or going in the sky
	if (TargetActor)
	{
		if (TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			if (LastDirection.Z < 0 && !ICombatInterface::Execute_GetIsAlive(TargetActor))
			{
				LastDirection.Z = 0;
			}
		}

	} 
	else LastDirection.Z = 0;

	FHitResult Hit;
	AddActorWorldOffset(LastDirection * SpellManager->CurrentSpellInfo.Speed * World->GetDeltaSeconds(), false, &Hit, ETeleportType::None);
}

void ABaseSpell::Finish()
{
	//SetActorTickEnabled(false);
	ClearAllTimers();
	UWorld* World = GetWorld();

	if (!World || !SpellManager)
	{
		Reset_Implementation();
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
	if (OverlappedComp == BaseCollider && OtherActor->Implements<UCombatInterface>() && OtherActor != SpellManager->GetCaster() && OtherActor != this && !CollidedActors.Contains(OtherActor))
	{
		ICombatInterface::Execute_OnCollidedWithSpell(OtherActor, this);
		CollidedActors.Add(OtherActor);

		if(VFXComponent) VFXComponent->StartHitVFX();
		
		Finish();
	}
}

void ABaseSpell::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ABaseSpell::Start_Implementation()
{
	// Note: Spell manager is null the first time!

	UWorld* World = GetWorld();
	if (!World) return;

	ClearAllTimers();
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	World->GetTimerManager().SetTimer(SetupCollisionTimerHandle, this, &ABaseSpell::SetupCollision, 0.075f, true);

	VFXComponent->StartMainVFX();
	CollidedActors.Empty();

	SetWatchdogTimers();
	
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

	World->GetTimerManager().SetTimer(UpdateDirectionTimerHandle, this, &ABaseSpell::UpdateMoveDirection, 0.05f, true);
	World->GetTimerManager().SetTimer(CheckTargetTimerHandle, this, &ABaseSpell::CheckTarget, 0.25f, true);
	CheckTarget();
	UpdateMoveDirection();
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
	SpellMovementComponent = CreateDefaultSubobject<USpellMovementComponent>(FName(TEXT("SpellMovementComponent")));

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

void ABaseSpell::UpdateMoveDirection()
{
	if (SpellMovementComponent)
	{
		LastDirection = SpellMovementComponent->GetMoveDirection(LastDirection);
		//LastDirection = FVector(0.5f, 0, 0);
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
