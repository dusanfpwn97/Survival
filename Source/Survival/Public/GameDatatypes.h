// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameDatatypes.generated.h"


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
