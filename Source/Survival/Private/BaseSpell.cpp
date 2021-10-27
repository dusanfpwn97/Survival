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
	UWorld* World = GetWorld();
	if (!World) return;
	if (TargetActor)
	{
		if (TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			if (!ICombatInterface::Execute_GetIsAlive(TargetActor) && !FinishTimerHandle.IsValid())
			{
				World->GetTimerManager().SetTimer(FinishTimerHandle, this, &ABaseSpell::Finish, 2.f, false);
			}
		}
	}
	else if(!FinishTimerHandle.IsValid()) //TODO
	{
		World->GetTimerManager().SetTimer(FinishTimerHandle, this, &ABaseSpell::Finish, 2.f, false);
	}

}

void ABaseSpell::Move()
{
	UWorld* World = GetWorld();
	if (!World) return;


	// If target is not valid, z = 0 so that spell keeps going on without hitting the ground or going in the sky
	if (TargetActor)
	{
		if (LastDirection.Z < 0 && !ICombatInterface::Execute_GetIsAlive(TargetActor))
		{
			LastDirection.Z = 0;
		}
	} else LastDirection.Z = 0;

	FHitResult Hit;
	AddActorWorldOffset(LastDirection * SpellManager->CurrentSpellInfo.Speed * World->GetDeltaSeconds(), false, &Hit, ETeleportType::None);
}

void ABaseSpell::Finish()
{
	SetActorTickEnabled(false);
	ClearAllTimers();
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &ABaseSpell::Reset_Implementation, 1.f, true);
	TargetActor = nullptr;
	RemoveCollision();
	MainNiagaraFX->Deactivate();
}

UBaseSpellManager* ABaseSpell::GetSpellManager() const
{
	return SpellManager;
}

void ABaseSpell::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp == BaseCollider && OtherActor->Implements<UCombatInterface>() && OtherActor != SpellManager->GetCaster() && OtherActor != this)
	{
		ICombatInterface::Execute_OnCollidedWithSpell(OtherActor, this);
		Finish();
		SpellManager->SpawnHitParticle(GetActorLocation());

	}
}

void ABaseSpell::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ABaseSpell::Start_Implementation()
{
	ClearAllTimers();
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	SetupCollision();
	MainNiagaraFX->Activate(true);


	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &ABaseSpell::Reset_Implementation, 10.f, true); // Safety
}

void ABaseSpell::Reset_Implementation()
{
	Finish();
	ClearAllTimers();

	SetActorHiddenInGame(true);

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
		FSpellInfo TempSpellInfo = SpellManager->CurrentSpellInfo;
		UNiagaraSystem* NGSystem = SpellManager->GetNiagaraSystem(TempSpellInfo.Element, TempSpellInfo.CastType, SpellFXType::MAIN);
		MainNiagaraFX->SetAsset(NGSystem);
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

	MainNiagaraFX = CreateDefaultSubobject<UNiagaraComponent>(FName(TEXT("NGParticle")));
	SpellMovementComponent = CreateDefaultSubobject<USpellMovementComponent>(FName(TEXT("SpellMovementComponent")));


	BaseCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseSpell::OnOverlapBegin);
	BaseCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseSpell::OnOverlapEnd);

	MainNiagaraFX->SetupAttachment(RootComponent);

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
	}
}

void ABaseSpell::UpdateMoveDirection()
{
	if (SpellMovementComponent)
	{
		LastDirection = SpellMovementComponent->GetMoveDirection(LastDirection);
	}
}
