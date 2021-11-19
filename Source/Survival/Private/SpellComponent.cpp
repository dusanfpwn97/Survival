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
	UWorld* World = GetWorld();
	if (!World) return;

	FTransform TempTransform;
	TempTransform.SetLocation(FVector(0, 0, 500));
	TempTransform.SetScale3D(FVector(1, 1, 1));
	TempTransform.SetRotation(FRotator(0, 0, 0).Quaternion());
	FActorSpawnParameters Params;

	ABaseSpellManager* NewSpell = World->SpawnActor<ABaseSpellManager>(ABaseSpellManager::StaticClass(), TempTransform, Params);

	SpellManagers.Add(NewSpell);
	NewSpell->InitSpellManager(SpellInfo);
	NewSpell->Caster = GetOwner();

}