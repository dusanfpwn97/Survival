// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FakeShadowDistributer.generated.h"


class USkeletalMeshComponent;


USTRUCT()
struct FShadowInfo
{
	GENERATED_BODY()

	UPROPERTY()
		AActor* Actor;
	int32 Index;
	FVector Offset;
	UPROPERTY()
		USkeletalMeshComponent* SkelMesh;

	FName BoneToFollow;
};

UCLASS()
class SURVIVAL_API AFakeShadowDistributer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFakeShadowDistributer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void SetStaticMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UInstancedStaticMeshComponent* ISM_Comp;

	void AssignNewShadow(AActor* Actor, FVector Offset, USkeletalMeshComponent* SkelMesh, FName BoneName);

	TArray<FShadowInfo> ShadowsInfo;

	void UpdateShadowsLocation();
};
