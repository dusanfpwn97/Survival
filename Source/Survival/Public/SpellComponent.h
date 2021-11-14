// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellDataTypes.h"
#include "SpellComponent.generated.h"


class UBaseSpellManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVAL_API USpellComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellComponent();

	UFUNCTION(BlueprintCallable)
		void AddNewSpell(FSpellInfo SpellInfo);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UBaseSpellManager*> SpellManagers;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
