// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellMovementComponent.h"
#include "BaseSpell.h"
#include "CombatComponent.h"
#include "BaseSpellManager.h"
#include "SpellDatatypes.h"

// Sets default values for this component's properties
USpellMovementComponent::USpellMovementComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
	SetComponentTickEnabled(false);
	
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

	if (!SpellOwner || !World || !SpellOwner->SpellManager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("SpellOwner not valid! Should not happen USpellMovementComponent::GetMoveDirection()"));
		return CurrentDirection;
	}
	FSpellInfo SpellInfo = SpellOwner->SpellManager->CurrentSpellInfo;

	if (hasset) return CurrentDirection;
	if (SpellInfo.CastType == CastType::STORM)
	{
		//return FVector(0.5f, -0.5, -1.f);
		hasset = true;
			return FVector(FMath::FRandRange(-0.4, -0.0), FMath::FRandRange(0.0, 0.4f), -1.f);
			return FVector(FMath::FRandRange(-0.5, 0.5), FMath::FRandRange(-0.5, 0.5f), -1.f);
	}

	if (!SpellOwner->TargetActor || SpellOwner->SpellManager->GetIsStaticLocationSpell()) return CurrentDirection;



	FVector NewDirection;


	if (SpellOwner->TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
	{
		if (ICombatInterface::Execute_GetIsAlive(SpellOwner->TargetActor))
		{
			//if(SpellInfo.CastType == CastType::FLICK)
			NewDirection = SpellOwner->TargetActor->GetActorLocation() - SpellOwner->GetActorLocation();
			NewDirection.Normalize();
			return NewDirection;
		}
		else return CurrentDirection;
	}

	return CurrentDirection;
}