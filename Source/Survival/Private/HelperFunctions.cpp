// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions.h"
#include "CombatInterface.h"



FVector UHelperFunctions::GetRandomPointInCircle(FVector Center, float Radius)
{
	FVector Location;
	float rand = FMath::FRand();
	float RandPoint = rand * 2 * 3.1415f;
	Location.X = Center.X + Radius * FMath::Cos(RandPoint);
	Location.Y = Center.Y + Radius * FMath::Sin(RandPoint);
	Location.Z = Center.Z;
	return Location;
}

AActor* UHelperFunctions::GetClosestActor(TArray<AActor*> Actors, FVector ReferenceLocation)
{
	if (Actors.Num() == 0) return nullptr;
	if (Actors.Num() == 1) return Actors[0];

	uint32 ClosestActorIndex = 0;
	float ClosestDistance = 999999.f;

	for (int i = 0; i <= Actors.Num() - 1; i++)
	{
		float TempDistance = FVector::Distance(Actors[i]->GetActorLocation(), ReferenceLocation);
		if(ClosestDistance > TempDistance)
		{	
			ClosestDistance = TempDistance;
			ClosestActorIndex = i;
		}
	}
	
	return Actors[ClosestActorIndex];
}


TArray<AActor*> UHelperFunctions::GetAllAliveActors(TArray<AActor*> ActorsToCheck)
{
	TArray<AActor*> AliveActors;

	for (AActor* Actor : ActorsToCheck)
	{
		if (Actor)
		{
			if (Actor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
			{
				if (ICombatInterface::Execute_GetIsAlive(Actor))
				{
					AliveActors.Add(Actor);
				}
			}
		}
	}
	return AliveActors;
}
FString UHelperFunctions::GetElementName(Element Element)
{
	if (Element == Element::FIRE) return "Fire";
	if (Element == Element::DARK) return "Dark";
	if (Element == Element::EARTH) return "Earth";
	if (Element == Element::ELECTRICITY) return "Electricity";
	if (Element == Element::ICE) return "Ice";
	if (Element == Element::NO_ELEMENT) return "No Element";
	if (Element == Element::POISON) return "Poison";
	if (Element == Element::WATER) return "Water";
	if (Element == Element::WIND) return "Wind";

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Non-existant Element Enum!  UHelperFunctions::GetElementName")));
	UE_LOG(LogTemp, Warning, TEXT("Non-existant Element Enum!  UHelperFunctions::GetElementName"));
	
	return FString();
}
FString UHelperFunctions::GetCastTypeName(CastType CastType)
{
	if (CastType == CastType::BEAM) return "Beam";
	if (CastType == CastType::FLICK) return "Flick";
	if (CastType == CastType::NOVA) return "Nova";
	if (CastType == CastType::NO_CAST_TYPE) return "No Cast Type";
	if (CastType == CastType::PROJECTILE) return "Projectile";
	if (CastType == CastType::SELF) return "Self";
	if (CastType == CastType::STORM) return "Storm";

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Non-existant CastType Enum!  UHelperFunctions::GetCastTypeName")));
	UE_LOG(LogTemp, Warning, TEXT("Non-existant CastType Enum!  UHelperFunctions::GetCastTypeName"));

	return FString();
}
FString UHelperFunctions::GetSpellFXTypeName(SpellFXType SpellFXType)
{
	if (SpellFXType == SpellFXType::MAIN) return "Main";
	if (SpellFXType == SpellFXType::ON_HIT) return "Hit";
	if (SpellFXType == SpellFXType::ON_SPAWN) return "OnSpawn";

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Non-existant SpellFXType Enum!  UHelperFunctions::GetSpellFXTypeName")));
	UE_LOG(LogTemp, Warning, TEXT("Non-existant SpellFXType Enum!  UHelperFunctions::GetSpellFXTypeName"));

	return FString();
}
TArray<CastType> UHelperFunctions::GetAllTargetlessCastTypes()
{
	TArray<CastType> Temp;
	Temp.Add(CastType::NOVA);
	Temp.Add(CastType::SELF);

	return Temp;
}
/*
void UHelperFunctions::AddSpell(AActor* Caster, UBaseSpellManager* SpellClass)
{
	FTransform SpawnTransform(FRotator::ZeroRotator, Origin);
	auto AActor* Actor = Cast<UBaseSpellManager>(UGameplayStatics::SpawnActorDeferred(this, SpellClass, SpawnTransform));
	if (MyDeferredActor != nullptr)
	{
		MyDeferredActor->Init(ShootDir);

		UGameplayStatics::FinishSpawningActor(MyDeferredActor, SpawnTransform);
	}
}


*/

