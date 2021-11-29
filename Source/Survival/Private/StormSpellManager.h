// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpellManager.h"
#include "StormSpellManager.generated.h"

/**
 * 
 */
UCLASS()
class AStormSpellManager : public ABaseSpellManager
{
	GENERATED_BODY()
public:

	AStormSpellManager();
	virtual void Tick(float DeltaTime) override;

	virtual void UpdateDirection(const int Index) override;

	virtual void UpdateInstanceTransforms() override;

	FVector GetDirectionOffsetForSplit(const int Index);

};
