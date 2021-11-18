// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellFlick.h"
#include "BaseSpellManager.h"

ASpellFlick::ASpellFlick()
{

}

void ASpellFlick::Move()
{
	UWorld* World = GetWorld();
	if (!World || !SpellManager) return;

	FHitResult Hit;
	CurrentDirection = GetMoveDirection();
	AddActorWorldOffset(CurrentDirection * SpellManager->CurrentSpellInfo.Speed * World->GetDeltaSeconds(), false, &Hit, ETeleportType::None);
}

FVector ASpellFlick::GetMoveDirection()
{
	if (!TargetActor)
	{
		if(HasDeterminedDirection) return CurrentDirection;

		if (CurrentDirection.IsNearlyZero(0.01))
		{
			HasDeterminedDirection = true;

			CurrentDirection = SpellManager->Caster->GetActorForwardVector();
		}
		return CurrentDirection;
	}

	if (TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
	{
		if (ICombatInterface::Execute_GetIsAlive(TargetActor))
		{
			FVector NewDirection = TargetActor->GetActorLocation() - GetActorLocation();
			NewDirection.Normalize();
			return NewDirection;
		}
		else if (CurrentDirection.Z < 0)
		{
			CurrentDirection.Z = 0;
			return CurrentDirection;
		}
		else return CurrentDirection;

	}
	return CurrentDirection;
}

void ASpellFlick::Start_Implementation()
{
	Super::Start_Implementation();

}

void ASpellFlick::UpdateTarget()
{

	if (!SpellManager) return;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Spell qwdqwdqwdasww"));

	if (!SpellManager->Caster) return;
	if (!SpellManager->Caster->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) return;

	if (SpellManager->CurrentSpellInfo.TargetMode == TargetMode::CLOSEST)
	{

		TargetActor = ICombatInterface::Execute_GetClosestEnemy(SpellManager->Caster);
	}

	UWorld* World = GetWorld();
	if (!World) return;

	//World->GetTimerManager().SetTimer(UpdateDirectionTimerHandle, this, &ABaseSpell::UpdateMoveDirection, 0.05f, true);
	World->GetTimerManager().SetTimer(CheckTargetTimerHandle, this, &ASpellFlick::CheckTarget, 0.25f, true);
	CheckTarget();
}


void ASpellFlick::CheckTarget()
{
	UWorld* World = GetWorld();
	if (!World) return;
	if (TargetActor)
	{
		if (TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			if (!ICombatInterface::Execute_GetIsAlive(TargetActor) && !FinishTimerHandle.IsValid())
			{
				World->GetTimerManager().SetTimer(FinishTimerHandle, this, &ASpellFlick::Finish, 8.f, false);
				//World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
				TargetActor = nullptr; // TODO look into how to optimize. Maybe some timers are called during these 8 seconds
			}
		}
	}
	else if (!FinishTimerHandle.IsValid()) //TODO
	{
		World->GetTimerManager().SetTimer(FinishTimerHandle, this, &ASpellFlick::Finish, 8.f, false);
		//World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
	}

}

void ASpellFlick::SetSpellManager_Implementation(UBaseSpellManager* NewSpellManager)
{
	Super::SetSpellManager_Implementation(NewSpellManager);

	UpdateTarget();
}
