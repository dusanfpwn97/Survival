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
}

void ABaseSpell::Move()
{
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

}

void ABaseSpell::Reset_Implementation()
{
	SetActorTickEnabled(false);
	Finish();
	ClearAllTimers();

	SetActorHiddenInGame(true);
	CollidedActors.Empty();
	HasDeterminedDirection = false;
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
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("PoolManager not valid! Should not happen. BaseEnemy.cpp -> SetSpawner_Implementation"));
		return;
	}
	CurrentPoolManager = Object;

}

void ABaseSpell::SetOrderIndex_Implementation(int NewOrderIndex)
{

	OrderIndex = NewOrderIndex;
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

void ABaseSpell::UpdateTarget()
{
}