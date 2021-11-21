// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"

bool USpellFunctionLibrary::IsSpellModifierCompatible(SpellModifier SpellModifierToCheck, CastType CastType)
{
	if (GetCompatibleSpellModifiers(CastType).Contains(SpellModifierToCheck))
	{
		return true;
	}

	return false;
}

TArray<SpellModifier> USpellFunctionLibrary::GetCompatibleSpellModifiers(CastType CastType)
{
	TArray<SpellModifier> Temp;

	if (CastType == CastType::BEAM)
	{
		Temp.Add(SpellModifier::AILMENT);
		Temp.Add(SpellModifier::LEAVE_TRAIL);
		Temp.Add(SpellModifier::PENETRATE);
		Temp.Add(SpellModifier::SPLIT);
		return Temp;
	}
	
	if (CastType == CastType::FLICK)
	{
		Temp.Add(SpellModifier::AILMENT);
		Temp.Add(SpellModifier::BOUNCE);
		Temp.Add(SpellModifier::LEAVE_TRAIL);
		Temp.Add(SpellModifier::EXPLODE_ON_IMPACT);
		Temp.Add(SpellModifier::SPLIT);
		return Temp;
	}
	
	if (CastType == CastType::NOVA)
	{
		Temp.Add(SpellModifier::AILMENT);
		Temp.Add(SpellModifier::LEAVE_TRAIL);
		return Temp;
	}

	if (CastType == CastType::PROJECTILE)
	{
		Temp.Add(SpellModifier::AILMENT);
		Temp.Add(SpellModifier::PENETRATE);
		Temp.Add(SpellModifier::LEAVE_TRAIL);
		Temp.Add(SpellModifier::EXPLODE_ON_IMPACT);
		Temp.Add(SpellModifier::SPLIT);
		return Temp;
	}

	if (CastType == CastType::SHIELD)
	{
		Temp.Add(SpellModifier::AILMENT);
		Temp.Add(SpellModifier::EXPLODE_ON_IMPACT);
		Temp.Add(SpellModifier::LEAVE_TRAIL);
		return Temp;
	}

	if (CastType == CastType::STORM)
	{
		Temp.Add(SpellModifier::AILMENT);
		Temp.Add(SpellModifier::EXPLODE_ON_IMPACT);
		Temp.Add(SpellModifier::LEAVE_TRAIL);
		Temp.Add(SpellModifier::SPLIT);
		return Temp;
	}

	return Temp;
}


FString USpellFunctionLibrary::GetElementName(Element Element)
{
	if (Element == Element::FIRE) return "Fire";
	if (Element == Element::VOID) return "Void";
	if (Element == Element::EARTH) return "Earth";
	if (Element == Element::ELECTRICITY) return "Electricity";
	if (Element == Element::ICE) return "Ice";
	if (Element == Element::NO_ELEMENT) return "No Element";
	if (Element == Element::WATER) return "Water";
	if (Element == Element::AIR) return "Air";

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Non-existant Element Enum!  UHelperFunctions::GetElementName")));
	UE_LOG(LogTemp, Warning, TEXT("Non-existant Element Enum!  USpellFunctionLibrary::GetElementName"));

	return FString();
}

FString USpellFunctionLibrary::GetCastTypeName(CastType CastType)
{
	if (CastType == CastType::BEAM) return "Beam";
	if (CastType == CastType::FLICK) return "Flick";
	if (CastType == CastType::NOVA) return "Nova";
	if (CastType == CastType::NO_CAST_TYPE) return "No Cast Type";
	if (CastType == CastType::PROJECTILE) return "Projectile";
	if (CastType == CastType::SHIELD) return "Shield";
	if (CastType == CastType::STORM) return "Storm";

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Non-existant CastType Enum!  UHelperFunctions::GetCastTypeName")));
	UE_LOG(LogTemp, Warning, TEXT("Non-existant CastType Enum!  USpellFunctionLibrary::GetCastTypeName"));

	return FString();
}

FString USpellFunctionLibrary::GetSpellFXTypeName(SpellFXType SpellFXType)
{
	if (SpellFXType == SpellFXType::MAIN) return "Main";
	if (SpellFXType == SpellFXType::ON_HIT) return "Hit";
	if (SpellFXType == SpellFXType::ON_SPAWN) return "OnSpawn";

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Non-existant SpellFXType Enum!  UHelperFunctions::GetSpellFXTypeName")));
	UE_LOG(LogTemp, Warning, TEXT("Non-existant SpellFXType Enum!  USpellFunctionLibrary::GetSpellFXTypeName"));

	return FString();
}

FString USpellFunctionLibrary::GetSpellModifierName(SpellModifier SpellModifier)
{
	if (SpellModifier == SpellModifier::AILMENT) return "Inflict Ailment";
	if (SpellModifier == SpellModifier::BOUNCE) return "Bounce to a nearest target";
	if (SpellModifier == SpellModifier::EXPLODE_ON_IMPACT) return "Explode on Impact";
	if (SpellModifier == SpellModifier::LEAVE_TRAIL) return "Leave a trail";
	if (SpellModifier == SpellModifier::NO_MODIFIER) return "No Modifier";
	if (SpellModifier == SpellModifier::PENETRATE) return "Penetrate";
	if (SpellModifier == SpellModifier::SPLIT) return "Split";

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Non-existant SpellFXType Enum!  UHelperFunctions::GetSpellFXTypeName")));
	UE_LOG(LogTemp, Warning, TEXT("Non-existant SpellModifier Enum!  USpellFunctionLibrary::GetSpellModifierName"));

	return FString();
}

TArray<CastType> USpellFunctionLibrary::GetAllTargetlessCastTypes()
{
	TArray<CastType> Temp;
	Temp.Add(CastType::NOVA);
	Temp.Add(CastType::SHIELD);

	return Temp;
}