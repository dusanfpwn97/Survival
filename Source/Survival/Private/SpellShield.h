// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpell.h"
#include "SpellShield.generated.h"

/**
 * 
 */
UCLASS()
class ASpellShield : public ABaseSpell
{
	GENERATED_BODY()

	virtual void Move() override;
};
