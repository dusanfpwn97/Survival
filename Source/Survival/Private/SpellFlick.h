// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSpell.h"
#include "SpellFlick.generated.h"

/**
 * 
 */
UCLASS()
class ASpellFlick : public ABaseSpell
{
	GENERATED_BODY()

		ASpellFlick();


	virtual void Move() override;
	virtual FVector GetMoveDirection() override;
	virtual void Start_Implementation() override;
	virtual void UpdateTarget() override;

	virtual void CheckTarget() override;

	virtual void SetSpellManager_Implementation(ABaseSpellManager* NewSpellManager);
};
