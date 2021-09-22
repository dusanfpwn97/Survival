// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpell.h"
#include "CombatInterface.h"

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
	Start();

}


// Called every frame
void ABaseSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult Hit;
	AddActorWorldOffset(FVector(10.f, 0, 0), false, &Hit, ETeleportType::None);

}

void ABaseSpell::Start()
{
	SetActorTickEnabled(true);
}

void ABaseSpell::Reset()
{
	SetActorTickEnabled(true);
	TargetActor = nullptr;
}

bool ABaseSpell::Init()
{
	return true;
}

void ABaseSpell::MoveTowardsTarget()
{
	if (TargetActor)
	{

	}
}

void ABaseSpell::SetTarget(AActor* NewTarget)
{
	TargetActor = NewTarget;
}

void ABaseSpell::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_OnCollidedWithSpell(OtherActor, this);
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

	BaseCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BaseCollider->SetGenerateOverlapEvents(true);
	BaseCollider->SetCollisionProfileName(FName(TEXT("Spell")));
	BaseCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseSpell::OnOverlapBegin);
	BaseCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseSpell::OnOverlapEnd);

	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	NGParticle->SetupAttachment(RootComponent);
}