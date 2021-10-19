// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpell.h"
#include "CombatInterface.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "BaseSpellManager.h"

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

	MoveTowardsTarget();

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

void ABaseSpell::MoveTowardsTarget()
{
	UWorld* World = GetWorld();
	if (!World) return;
	if (!TargetActor) return; //TODO
	if (!TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) return;
	if (ICombatInterface::Execute_GetIsAlive(TargetActor))
	{
		LastDirection = TargetActor->GetActorLocation() - GetActorLocation();
		LastDirection.Normalize();

	}

	FHitResult Hit;
	if (LastDirection.Z < 0) LastDirection.Z = 0;
	AddActorWorldOffset(LastDirection * 800 * World->GetDeltaSeconds(), false, &Hit, ETeleportType::None);
}

void ABaseSpell::Finish()
{

	SetActorTickEnabled(false);
	UWorld* World = GetWorld();
	{
		World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(FinishTimerHandle, this, &ABaseSpell::Reset_Implementation, 1.f, true);
	
	RemoveCollision();
	MainNiagaraFX->Deactivate();

}

UBaseSpellManager* ABaseSpell::GetSpellManager() const
{
	return SpellManager;
}

void ABaseSpell::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp == BaseCollider && OtherActor->Implements<UCombatInterface>())
	{

		ICombatInterface::Execute_OnCollidedWithSpell(OtherActor, this);
		Finish();
	}
}

void ABaseSpell::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ABaseSpell::Start_Implementation()
{

	UWorld* World = GetWorld();
	{
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);
		World->GetTimerManager().ClearTimer(FinishTimerHandle);
		World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
	}
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	SetupCollision();
	MainNiagaraFX->Activate(true);

}

void ABaseSpell::Reset_Implementation()
{
	UWorld* World = GetWorld();
	{
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);
		World->GetTimerManager().ClearTimer(FinishTimerHandle);
		World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
	}
	SetActorTickEnabled(false);
	TargetActor = nullptr;
	SetActorHiddenInGame(true);
	RemoveCollision();

	MainNiagaraFX->Deactivate();
	if (CurrentPoolManager)
	{
		IPoolInterface::Execute_ReleaseToPool(CurrentPoolManager, this);
		
	}
	else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("CurrentPoolManager null BaseSpell::ResetImplementation!")));

}

void ABaseSpell::SetTarget_Implementation(AActor* NewTarget)
{
	TargetActor = NewTarget;
	UWorld* World = GetWorld();
	if (!World) return;

	GetWorld()->GetTimerManager().SetTimer(CheckTargetTimerHandle, this, &ABaseSpell::CheckTarget, 0.25f, true);
}

void ABaseSpell::SetSpellManager_Implementation(UBaseSpellManager* NewSpellManager)
{
	SpellManager = NewSpellManager;
	if (SpellManager)
	{
		FSpellInfo TempSpellInfo = SpellManager->GetSpellInfo();
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
	BaseCollider->SetGenerateOverlapEvents(true);
	BaseCollider->SetCollisionProfileName(FName(TEXT("Spell")));
}