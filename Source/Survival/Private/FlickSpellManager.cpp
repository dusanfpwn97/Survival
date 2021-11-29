// Fill out your copyright notice in the Description page of Project Settings.


#include "FlickSpellManager.h"
#include "CombatInterface.h"

AFlickSpellManager::AFlickSpellManager()
{

}

void AFlickSpellManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveSpells();
	CheckForCollisions();
	// ...

}

void AFlickSpellManager::UpdateDirection(const int Index)
{
	AActor* Target = SpellInstances[Index].Target;
	
	if (Target)
	{
		if (Target->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			ICombatInterface* TempInterface = Cast<ICombatInterface>(Target);

			if (TempInterface)
			{
				if (TempInterface->GetIsAlive())
				{
					SpellInstances[Index].CurrentDirection = Target->GetActorLocation() - SpellInstances[Index].Transform.GetLocation();
					SpellInstances[Index].CurrentDirection.Normalize();
					return;
				}
				else
				{
					SpellInstances[Index].Target = nullptr;

					SpellInstances[Index].HasGotInitialDirection = true;
					//FVector NewDirection = 
					SpellInstances[Index].CurrentDirection.Z = 0; // 
					SpellInstances[Index].CurrentDirection.Normalize();

					return;
				}
			}

		}
		else
		{
			// TODO if not enemy do something
			
		}

	}

	else
	{
		if (SpellInstances[Index].CurrentDirection.IsNearlyZero(0.01))
		{
			if (Caster)
			{
				SpellInstances[Index].HasGotInitialDirection = true;

				SpellInstances[Index].CurrentDirection = Caster->GetActorForwardVector();

				return;
			}
			else
			{
				SpellInstances[Index].HasGotInitialDirection = true;

				SpellInstances[Index].CurrentDirection = FVector(1.f, 0.f, 0.f);

				return;
			}
		}
	}
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
			SpellInstances[i].Velocity = (SpellInstances[i].CurrentDirection * CurrentSpellInfo.Speed * 1.06);
			//SpellInstances[i].Velocity = SpellInstances[i].Velocity.GetClampedToMaxSize(10.f);
			SpellInstances[i].Transform.SetRotation(SpellInstances[i].Velocity.Rotation().Quaternion());
			FVector NewLoc = SpellInstances[i].Transform.GetLocation() += SpellInstances[i].Velocity * World->DeltaTimeSeconds;
			NewLoc.Z = 100;
			SpellInstances[i].Transform.SetLocation(NewLoc);
		}
	}
}

void AFlickSpellManager::CastSpellLoop()
{
	FSpellRuntimeInfo Info;

	if (SpellModifiers.Contains(SpellModifier::SPLIT))
	{
		if (Caster)
		{
			TArray<AActor*> Targets = GetAppropriateTargets(5);

			for (int i = 0; i <= 4; i++)
			{
				AActor* Target = nullptr;
				
				if(Targets.Num() > 0)
				{
					if (Targets.Num() > i)
					{
						Target = Targets[i];
					}
					else
					{
						Target = Targets.Last();
					}

				}
				Target = Targets.Last();
				
				Info.OrderIndex = i;
				if(Target) Info.Target = Target;
				CastSpell(Info);
			}
			return;
		}
	}
	else
	{
		Info.Target = GetAppropriateTarget(-1, 0);
	}
	
	
	CastSpell(Info);
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