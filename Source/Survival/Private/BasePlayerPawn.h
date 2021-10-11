// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CombatInterface.h"

#include "BasePlayerPawn.generated.h"

class UCapsuleComponent;
class ABaseEnemy;
class USkeletalMeshComponent;
class UBaseSpellManager;

UCLASS(Blueprintable)
class ABasePlayerPawn : public APawn, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* MainCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMesh;
	UFUNCTION()
		void SetupComponents();

	UFUNCTION(BlueprintCallable)
		void AddNewSpell(TSoftClassPtr<UBaseSpellManager> SpellManagerClass);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UBaseSpellManager*> SpellManagers;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnCollidedWithEnemy_Implementation(ABaseEnemy* Enemy) override;

	UFUNCTION()
		virtual FVector GetSpellCastLocation_Implementation() override;

};
