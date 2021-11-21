// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpellDatatypes.generated.h"

class ABaseSpell;
class UNiagaraSystem;
class UStaticMesh;
class UMaterialInterface;

UENUM(BlueprintType)
enum class SpellModifier : uint8
{
	NO_MODIFIER,
	SPLIT,
	PENETRATE,
	BOUNCE,
	EXPLODE_ON_IMPACT,
	LEAVE_TRAIL,
	AILMENT
};

UENUM(BlueprintType)
enum class Element : uint8
{
	NO_ELEMENT,
	FIRE,
	WATER,
	AIR,
	ELECTRICITY,
	VOID,
	EARTH,
	ICE
};

UENUM(BlueprintType)
enum class CastType : uint8
{
	NO_CAST_TYPE,
	PROJECTILE,
	SHIELD,
	BEAM,
	NOVA,
	STORM,
	FLICK
};

UENUM(BlueprintType)
enum class SpellFXType : uint8
{
	MAIN,
	ON_HIT,
	ON_SPAWN
};

UENUM(BlueprintType)
enum class TargetMode : uint8
{
	CLOSEST,
	MIN_HEALTH,
	MAX_HEALTH,
	FARTHEST,
	NONE
};

USTRUCT(BlueprintType)
struct FSpellInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TSoftClassPtr<ABaseSpell> SpellClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Cooldown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Radius;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		Element Element;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		CastType CastType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		SpellModifier SpellModifier;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TargetMode TargetMode;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//FSpellFXInfo ParticlesInfo;
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

USTRUCT(BlueprintType)
struct FCastTypeAndElement : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		Element Element;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		CastType CastType;


};

USTRUCT(BlueprintType)
struct FSpellVFXInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCastTypeAndElement Binding;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UNiagaraSystem> MainFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UNiagaraSystem> SpawnFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UNiagaraSystem> HitFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UStaticMesh> MainMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UMaterialInterface> MainMaterial;

};

USTRUCT(NotBlueprintable)
struct FSpellRuntimeInfo
{
	GENERATED_BODY()
public:

	bool IsActive = false;
	int ISMIndex = 0;
	int OrderIndex = 0;
	FVector CurrentDirection;
	float SpawnTime = 0.f;
	FTransform Transform;

	TArray<AActor*> CollidedActors;
	FVector Velocity = FVector(0.f, 0.f, 0.f);
	bool HasGotInitialDirection = false;
	AActor* Target = nullptr;

	void Reset()
	{
		CollidedActors.Empty();
		IsActive = false;
		Transform.SetLocation(FVector(0.f, 0.f, 4000.f));
		Transform.SetRotation(FRotator(0, 0, 0).Quaternion());
		Target = nullptr;
		Velocity = FVector(0.f,0.f,0.f);
		HasGotInitialDirection = false;
	}
};

class SURVIVAL_API SpellDatatypes
{
public:
	SpellDatatypes();
	~SpellDatatypes();
};
