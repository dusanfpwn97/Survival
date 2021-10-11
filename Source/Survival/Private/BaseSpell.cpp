// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpell.h"
#include "CombatInterface.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"

// Sets default values
ABaseSpell::ABaseSpell()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetupComponents();

}

// Called when the game starts or when spawned
void ABaseSpell::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void ABaseSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult Hit;
	AddActorWorldOffset(FVector(400.f * DeltaTime, 0, 0), false, &Hit, ETeleportType::None);

}

void ABaseSpell::Start_Implementation()
{
	SetActorTickEnabled(true);
	DestroyTimerHandle.Invalidate();
	SetActorHiddenInGame(false);
	SetupCollision();
}

void ABaseSpell::Reset_Implementation()
{
	SetActorTickEnabled(false);
	TargetActor = nullptr;
	SetActorHiddenInGame(true);
	RemoveCollision();
}

void ABaseSpell::MoveTowardsTarget()
{
	if (TargetActor)
	{

	}
}

void ABaseSpell::Finish()
{
	SetActorTickEnabled(false);
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ABaseSpell::Reset_Implementation, 2.f, false);
	SetActorHiddenInGame(true);
}

void ABaseSpell::SetTarget_Implementation(AActor* NewTarget)
{
	TargetActor = NewTarget;
}

void ABaseSpell::SetSpellManager_Implementation(UBaseSpellManager* NewSpellManager)
{
	SpellManager = NewSpellManager;
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

void ABaseSpell::SetupComponents()
{
	BaseCollider = CreateDefaultSubobject<USphereComponent>(FName(TEXT("BaseCollider")));
	RootComponent = BaseCollider;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Mesh")));
	NGParticle = CreateDefaultSubobject<UNiagaraComponent>(FName(TEXT("NGParticle")));

	
	BaseCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseSpell::OnOverlapBegin);
	BaseCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseSpell::OnOverlapEnd);

	Mesh->SetupAttachment(RootComponent);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	
	NGParticle->SetupAttachment(RootComponent);

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
