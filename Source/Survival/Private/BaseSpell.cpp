// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpell.h"

// Sets default values
ABaseSpell::ABaseSpell()
{
	PrimaryActorTick.bCanEverTick = false;
	BaseCollider = CreateDefaultSubobject<USphereComponent>(FName(TEXT("BaseCollider")));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Mesh")));
	NGParticle = CreateDefaultSubobject<UNiagaraComponent>(FName(TEXT("NGParticle")));

	RootComponent = BaseCollider;
	Mesh->SetupAttachment(RootComponent);
	NGParticle->SetupAttachment(RootComponent);
	
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

}

void ABaseSpell::Init()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseSpell::Reset()
{
	PrimaryActorTick.bCanEverTick = false;
}
