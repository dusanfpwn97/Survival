// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpell.h"
#include "SpellStorm.generated.h"

/**
 * 
 */
UCLASS()
class ASpellStorm : public ABaseSpell
{
	GENERATED_BODY()
		ASpellStorm();


	virtual void Move() override;
	virtual FVector GetMoveDirection() override;
};
