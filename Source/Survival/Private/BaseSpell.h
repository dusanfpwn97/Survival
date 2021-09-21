// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"


#include "BaseSpell.generated.h"

UCLASS()
class ABaseSpell : public AActor
{
	GENERATED_BODY()
	
public:	

	ABaseSpell();
private:
	USphereComponent* BaseCollider;
	UStaticMeshComponent* Mesh;
	UNiagaraComponent* NGParticle;


protected:

	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void Init();
	UFUNCTION()
	void Reset();

	virtual void Tick(float DeltaTime) override;

};
