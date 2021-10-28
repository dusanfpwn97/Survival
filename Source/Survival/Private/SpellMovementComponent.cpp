// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellMovementComponent.h"
#include "BaseSpell.h"
#include "CombatComponent.h"
#include "BaseSpellManager.h"

// Sets default values for this component's properties
USpellMovementComponent::USpellMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USpellMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

FVector USpellMovementComponent::GetMoveDirection(FVector CurrentDirection)
{

	UWorld* World = GetWorld();

	if (!SpellOwner || !World || !SpellOwner->SpellManager || SpellOwner->SpellManager->GetIsTargetlessSpell())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("SpellOwner not valid! Should not happen USpellMovementComponent::GetMoveDirection()"));
		return CurrentDirection;
	}

	if (!SpellOwner->TargetActor) return CurrentDirection;
	if (!SpellOwner->TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) return CurrentDirection;
	
	FSpellInfo SpellInfo = SpellOwner->SpellManager->CurrentSpellInfo;
	FVector NewDirection;

	if (ICombatInterface::Execute_GetIsAlive(SpellOwner->TargetActor))
	{
		//if(SpellInfo.CastType == CastType::FLICK)
		NewDirection = SpellOwner->TargetActor->GetActorLocation() - SpellOwner->GetActorLocation();
		NewDirection.Normalize();
		return NewDirection;
	}
	else return CurrentDirection;
}