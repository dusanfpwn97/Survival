// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HelperFunctions.generated.h"

/**
 * 
 */
UCLASS()
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
};
