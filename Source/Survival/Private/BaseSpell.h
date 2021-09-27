// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolInterface.h"

class UNiagaraComponent;
class USphereComponent;

#include "BaseSpell.generated.h"

UCLASS(Blueprintable)
class ABaseSpell : public AActor, public IPoolInterface
{
	GENERATED_BODY()
	
public:	

	ABaseSpell();
private:
	UFUNCTION()
	void SetupComponents();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* BaseCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UNiagaraComponent* NGParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* TargetActor;

	UFUNCTION()
		void MoveTowardsTarget();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// In the constructor make sure to use AddDynamic to handle this function dynamically


public:	
	UFUNCTION()
		void Start_Implementation() override;
	UFUNCTION()
		void Reset_Implementation() override;

	UFUNCTION()
		void SetTarget(AActor* NewTarget);

	virtual void Tick(float DeltaTime) override;


};
