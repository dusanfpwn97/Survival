// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemySpawner.h"
#include "HelperFunctions.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}


/*
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}*/

void UCombatComponent::TakeDamage(float Damage)
{
	Health -= Damage;

	if (Health <= 0)
	{
		IsAlive = false;

		if (GetOwner()->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			ICombatInterface* TempInterface = Cast<ICombatInterface>(GetOwner());
			if (!TempInterface) return;

			TempInterface->OnDeath();
		}
	}
	else
	{

	}
}

void UCombatComponent::Init(float NewHealth)
{
	IsAlive = true;
	Health = Health;
}
