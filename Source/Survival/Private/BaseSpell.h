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
		void Start();
	UFUNCTION()
		void Reset();
	UFUNCTION()
		bool Init();
	UFUNCTION()
		void SetTarget(AActor* NewTarget);

	virtual void Tick(float DeltaTime) override;

};
