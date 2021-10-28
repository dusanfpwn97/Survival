// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpellDatatypes.h"
#include "HelperFunctions.generated.h"


class UBaseSpellManager;
UCLASS(BlueprintType)
class SURVIVAL_API UHelperFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION()
		static FVector GetRandomPointInCircle(FVector Center, float Radius);
	UFUNCTION()
		static AActor* GetClosestActor(TArray<AActor*> Actors, FVector ReferenceLocation);
	UFUNCTION()
		static TArray<AActor*> GetAllAliveActors(TArray<AActor*> ActorsToCheck);
	UFUNCTION(BlueprintCallable)
		static FString GetElementName(Element Element);
	UFUNCTION(BlueprintCallable)
		static FString GetCastTypeName(CastType CastType);
	UFUNCTION(BlueprintCallable)
		static FString GetSpellFXTypeName(SpellFXType SpellFXType);
	UFUNCTION(BlueprintCallable)
		static TArray<CastType> GetAllTargetlessCastTypes();

};
