// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpellManager.h"
#include "ShieldSpellManager.generated.h"

/**
 * 
 */
UCLASS()
class AShieldSpellManager : public ABaseSpellManager
{
	GENERATED_BODY()

public:

	AShieldSpellManager();
	virtual void Tick(float DeltaTime) override;

	virtual void UpdateInstanceTransforms() override;

	float WatchdogTimeThreshold = 99999.f;
};
