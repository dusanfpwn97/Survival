// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpellDatatypes.h"
#include "SpellFunctionLibrary.generated.h"

class UDataTable;

/**
 * 
 */
UCLASS()
class USpellFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		static bool IsSpellModifierCompatible(SpellModifier SpellModifierToCheck, CastType CastType);
	UFUNCTION(BlueprintCallable)
		static TArray<SpellModifier> GetCompatibleSpellModifiers(CastType CastType);
	UFUNCTION(BlueprintCallable)
		static FString GetElementName(Element Element);
	UFUNCTION(BlueprintCallable)
		static FString GetCastTypeName(CastType CastType);
	UFUNCTION(BlueprintCallable)
		static FString GetSpellFXTypeName(SpellFXType SpellFXType);
	UFUNCTION(BlueprintCallable)
		static FString GetSpellModifierName(SpellModifier SpellModifier);
	UFUNCTION(BlueprintCallable)
		static TArray<CastType> GetAllTargetlessCastTypes();

};
