// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpellDatatypes.h"
#include "HelperFunctions.generated.h"


class ABaseSpellManager;
UCLASS()
class SURVIVAL_API UHelperFunctions : public UObject
{
	GENERATED_BODY()
public:

	static FVector GetRandomPointInCircle(FVector Center, float Radius);

	static AActor* GetClosestActor(TArray<AActor*> Actors, FVector ReferenceLocation);

	static TArray<AActor*> GetClosestActors(TArray<AActor*> Actors, FVector ReferenceLocation, int32 NumOfActors);

	//static AActor* GetClosestActor(TArray<AActor*> Actors, FVector ReferenceLocation);

	static TArray<AActor*> GetAllAliveActors(TArray<AActor*> ActorsToCheck);

};
