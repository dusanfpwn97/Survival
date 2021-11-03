// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellComponent.h"
#include "BaseSpellManager.h"

// Sets default values for this component's properties
USpellComponent::USpellComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpellComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USpellComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USpellComponent::AddNewSpell(FSpellInfo SpellInfo)
{

	UBaseSpellManager* NewSpell = NewObject<UBaseSpellManager>(this, UBaseSpellManager::StaticClass()); //*

	//NewSpell->OnComponentCreated();
	NewSpell->RegisterComponent();
	SpellManagers.Add(NewSpell);
	NewSpell->InitSpellManager(SpellInfo);
	NewSpell->Caster = GetOwner();
}