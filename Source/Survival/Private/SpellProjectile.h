// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpell.h"
#include "SpellProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ASpellProjectile : public ABaseSpell
{
	GENERATED_BODY()

	ASpellProjectile();


	virtual void Move() override;
	virtual FVector GetMoveDirection() override;
};
