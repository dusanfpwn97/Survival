// Fill out your copyright notice in the Description page of Project Settings.


#include "FlickSpellManager.h"
#include "CombatInterface.h"

AFlickSpellManager::AFlickSpellManager()
{

}

FVector AFlickSpellManager::UpdateDirection(const int Index)
{
	AActor* Target = SpellInstances[Index].Target;
	if (Target)
	{
		if (Target->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			ICombatInterface* TempInterface = Cast<ICombatInterface>(Target);

			if (TempInterface->GetIsAlive())
			{
				SpellInstances[Index].CurrentDirection = Target->GetActorLocation() - SpellInstances[Index].Transform.GetLocation();
				SpellInstances[Index].CurrentDirection.Normalize();
				
				return SpellInstances[Index].CurrentDirection;
			}
			else
			{
				SpellInstances[Index].Target = nullptr;

				SpellInstances[Index].HasGotInitialDirection = true;
				//FVector NewDirection = 
				SpellInstances[Index].CurrentDirection.Z = 0; // 
				SpellInstances[Index].CurrentDirection.Normalize();
				return SpellInstances[Index].CurrentDirection;
			}
		}
		else
		{
			// TODO if not enemy do something
			return SpellInstances[Index].CurrentDirection;
		}

	}

	else
	{
		if (SpellInstances[Index].HasGotInitialDirection) return SpellInstances[Index].CurrentDirection;

		if (SpellInstances[Index].CurrentDirection.IsNearlyZero(0.01))
		{

			if (Caster)
			{
				SpellInstances[Index].HasGotInitialDirection = true;

				SpellInstances[Index].CurrentDirection = Caster->GetActorForwardVector();
			}
		}
		else
		{
			return SpellInstances[Index].CurrentDirection;
		}

	}
	return SpellInstances[Index].CurrentDirection;
}

void AFlickSpellManager::UpdateInstanceTransforms()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (int i = 0; i < SpellInstances.Num(); i++)
	{
		if (SpellInstances[i].IsActive)
		{
			UpdateDirection(i);
			SpellInstances[i].Velocity = (SpellInstances[i].CurrentDirection * CurrentSpellInfo.Speed * 1.06) * World->DeltaTimeSeconds;
			//SpellInstances[i].Velocity = SpellInstances[i].Velocity.GetClampedToMaxSize(10.f);
			SpellInstances[i].Transform.SetLocation(SpellInstances[i].Transform.GetLocation() += SpellInstances[i].Velocity);
		}
	}
}

void AFlickSpellManager::UpdateTarget(const int Index)
{
	if (!Caster) return;
	if (!Caster->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) return;
	ICombatInterface* TempInterface = Cast<ICombatInterface>(Caster);
	if (!TempInterface) return;

	if (CurrentSpellInfo.TargetMode == TargetMode::CLOSEST)
	{

		SpellInstances[Index].Target = TempInterface->GetClosestEnemy();
	}
	/*
	UWorld* World = GetWorld();
	if (!World) return;

	//World->GetTimerManager().SetTimer(UpdateDirectionTimerHandle, this, &ABaseSpell::UpdateMoveDirection, 0.05f, true);
	World->GetTimerManager().SetTimer(CheckTargetTimerHandle, this, &AFlickSpellManager::CheckTarget, 0.25f, true);
	CheckTarget();*/
}

/*
void AFlickSpellManager::CheckTarget()
{
	UWorld* World = GetWorld();
	if (!World) return;
	if (TargetActor)
	{
		if (!FinishTimerHandle.IsValid())
		{
			if (TargetActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
			{
				ICombatInterface* TempInterface = Cast<ICombatInterface>(TargetActor);
				if (TempInterface->GetIsAlive())
				{
					World->GetTimerManager().SetTimer(FinishTimerHandle, this, &ASpellFlick::Finish, 8.f, false);
					//World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
					TargetActor = nullptr; // TODO look into how to optimize. Maybe some timers are called during these 8 seconds

				}
			}
		}
	}
	else if (!FinishTimerHandle.IsValid()) //TODO
	{
		World->GetTimerManager().SetTimer(FinishTimerHandle, this, &ASpellFlick::Finish, 8.f, false);
		//World->GetTimerManager().ClearTimer(CheckTargetTimerHandle);
	}

}*/