// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpellManager.h"
#include "FlickSpellManager.generated.h"

/**
 * 
 */
UCLASS()
class AFlickSpellManager : public ABaseSpellManager
{
	GENERATED_BODY()
public:

	AFlickSpellManager();
	virtual FVector UpdateDirection(const int Index) override;

	virtual void UpdateInstanceTransforms() override;

	virtual void UpdateTarget(const int Index) override;

	FTimerHandle CheckTargetTimerHandle;
};
