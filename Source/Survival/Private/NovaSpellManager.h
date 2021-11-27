// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpellManager.h"
#include "NovaSpellManager.generated.h"

/**
 * 
 */
UCLASS()
class ANovaSpellManager : public ABaseSpellManager
{
	GENERATED_BODY()
public:
	ANovaSpellManager();
	virtual void Tick(float DeltaTime) override;

	virtual void UpdateInstanceTransforms() override;

	virtual void CheckForCollisions() override;

};
