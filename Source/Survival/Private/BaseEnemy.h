// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatInterface.h"
#include "PoolInterface.h"
#include "BaseEnemy.generated.h"

class ABasePlayerPawn;
class UCapsuleComponent;
class ABaseSpell;
class UEnemySpawner;

UCLASS(Blueprintable)
class ABaseEnemy : public AActor, public ICombatInterface, public IPoolInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* MainCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;

	UFUNCTION()
		void SetupComponents();

	UPROPERTY()
	FTimerHandle DestroyTimerHandle;

	UPROPERTY()
		bool bIsActive = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float temp;

	UFUNCTION()
	void OnCollidedWithSpell_Implementation(ABaseSpell* Spell) override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void Start_Implementation() override;
	UFUNCTION()
		void Reset_Implementation() override;
	UFUNCTION()
		void SetSpawner_Implementation(UObject* Object) override;
	
	UPROPERTY()
		UObject* Spawner;

};
