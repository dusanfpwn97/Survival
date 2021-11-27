// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellComponent.h"
#include "BaseSpellManager.h"
#include "ProjectileSpellManager.h"
#include "FlickSpellManager.h"
#include "StormSpellManager.h"
#include "ShieldSpellManager.h"
#include "NovaSpellManager.h"

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
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	UClass* ClassToSpawn = GetSpellClassForSpawning(SpellInfo.CastType);
	ABaseSpellManager* NewSpell = World->SpawnActor<ABaseSpellManager>(ClassToSpawn, TempTransform, Params);

	if (NewSpell)
	{
		SpellManagers.Add(NewSpell);
		NewSpell->Caster = GetOwner();
		NewSpell->InitSpellManager(SpellInfo);
	}
	else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No spell mennager class available.USpellComponent::GetSpellClassForSpawning(CastType CastType)"));



}

UClass* USpellComponent::GetSpellClassForSpawning(CastType CastType)
{
	if (CastType == CastType::PROJECTILE) return AProjectileSpellManager::StaticClass();
	if (CastType == CastType::FLICK) return AFlickSpellManager::StaticClass();
	if (CastType == CastType::STORM) return AStormSpellManager::StaticClass();
	if (CastType == CastType::SHIELD) return AShieldSpellManager::StaticClass();
	if (CastType == CastType::NOVA) return ANovaSpellManager::StaticClass();

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No spell mennager class available.USpellComponent::GetSpellClassForSpawning(CastType CastType)"));

	return nullptr;
}