// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameDatatypes.generated.h"

class ABaseSpell;

UENUM(BlueprintType)
enum class SpellModifier : uint8
{
	NO_MODIFIER,
	ADDITIONAL_PROJECTILES,
	PENETRATE_TARGET,
	ADDITIONAL_CAST,
	BOUNCE_TO_NEAREST,
	EXPLODE_ON_IMPACT,
	LEAVE_TRAIL,
	INFLICT_AILMENT
};

UENUM(BlueprintType)
enum class Element : uint8
{
	NO_ELEMENT,
	FIRE,
	WATER,
	WIND,
	ELECTRICITY,
	POISON,
	DARK,
	EARTH,
	COLD
};

UENUM(BlueprintType)
enum class CastType : uint8
{
	NO_CAST_TYPE,
	PROJECTILE,
	SELF,
	BEAM,
	NOVA,
	STORM,
	FLICK
};

UENUM(BlueprintType)
enum class AttackMode : uint8
{
	CLOSEST,
	MIN_HEALTH,
	MAX_HEALTH,
	FARTHEST
};

USTRUCT(BlueprintType)
struct FSpellInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UTexture2D> Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Cooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<Element> Elements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<CastType> CastTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<SpellModifier> SpellModifiers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftClassPtr<ABaseSpell> SpellClass;
};

USTRUCT(BlueprintType)
struct FCastTypes : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		CastType CastType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UTexture2D> Texture;

};

USTRUCT()
struct FPooledActors
{
	GENERATED_BODY()

	UPROPERTY()
		TArray<AActor*> Actors;
	UPROPERTY()
		TArray<int32> AvailableIndices;
};

class SURVIVAL_API GameDatatypes
{
public:
	GameDatatypes();
	~GameDatatypes();
};
