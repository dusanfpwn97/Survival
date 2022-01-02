// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatInterface.h"

#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVAL_API UCombatComponent : public UActorComponent, public ICombatInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();


	float Health = 100.f;
	bool IsAlive = true;

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(float NewHealth);
	void TakeDamage(float Damage);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;



};
