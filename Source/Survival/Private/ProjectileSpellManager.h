// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpellManager.h"
#include "ProjectileSpellManager.generated.h"

/**
 * 
 */
UCLASS()
class AProjectileSpellManager : public ABaseSpellManager
{
	GENERATED_BODY()
public:

	AProjectileSpellManager();
	virtual void Tick(float DeltaTime) override;

	virtual void UpdateDirection(const int Index) override;

	FVector GetDirectionForSplit(const int Index);

	virtual void UpdateInstanceTransforms() override;
};
