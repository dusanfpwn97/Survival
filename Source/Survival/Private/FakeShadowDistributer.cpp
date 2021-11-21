// Fill out your copyright notice in the Description page of Project Settings.


#include "FakeShadowDistributer.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AFakeShadowDistributer::AFakeShadowDistributer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ISM_Comp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(TEXT("ISM_Comp")));
	RootComponent = ISM_Comp;
	SetStaticMesh();
	ISM_Comp->SetCastShadow(false);
	ISM_Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ISM_Comp->SetGenerateOverlapEvents(false);
	ISM_Comp->SetCollisionProfileName(FName("None"));
	ISM_Comp->UpdateBounds();
}

// Called when the game starts or when spawned
void AFakeShadowDistributer::BeginPlay()
{
	Super::BeginPlay();
	

}

void AFakeShadowDistributer::SetStaticMesh()
{
	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Enviroment/FakeShadow/SM_FakeMeshPlane.SM_FakeMeshPlane'"));
	UStaticMesh* Asset = MeshAsset.Object;
	if (Asset)
	{
		ISM_Comp->SetStaticMesh(Asset);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Yellow, TEXT("No fake shadow mesh found!"));
	}
}

// Called every frame
void AFakeShadowDistributer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateShadowsLocation();
}

void AFakeShadowDistributer::AssignNewShadow(AActor* Actor, FVector Offset, USkeletalMeshComponent* SkelMesh, FName BoneName)
{
	//int32 TempLength = ShadowMap.Num();

	FTransform Transform;
	Transform.SetScale3D(FVector(1, 1, 1));
	Transform.SetLocation(Actor->GetActorLocation() + Offset);
	int32 InstIndex = ISM_Comp->AddInstanceWorldSpace(Transform);
	FShadowInfo Info;
	Info.Actor = Actor;
	Info.Index = InstIndex;
	Info.Offset = Offset;
	Info.SkelMesh = SkelMesh;
	Info.BoneToFollow = BoneName;
//	ShadowMap.Add(Actor, Index);
	ShadowsInfo.Add(Info);
}

void AFakeShadowDistributer::UpdateShadowsLocation()
{
	TArray<int32> IndexesToHide;
	for (int i = 0; i < ShadowsInfo.Num(); i++)
	{
		if (ShadowsInfo[i].Actor)
		{
			bool IsDirty = false;
			FTransform Transform;
			ISM_Comp->GetInstanceTransform(ShadowsInfo[i].Index, Transform);
			FVector ShadowSocketPosition = FVector::ZeroVector;
			if (ShadowsInfo[i].SkelMesh)
			{
				ShadowSocketPosition = ShadowsInfo[i].SkelMesh->GetSocketLocation(ShadowsInfo[i].BoneToFollow);
				if (ShadowSocketPosition.Z < 0)
				{
					//IndexesToHide.Add(i);
					Transform.SetScale3D(FVector(0.01, 0.01, 0.01));
					ShadowSocketPosition.Z = 5000; // Set shadow to be a bit off the ground
				}
				else
				{
					Transform.SetScale3D(FVector(1, 1, 1));
					ShadowSocketPosition.Z = 2; // Set shadow to be a bit off the ground
				}
				
			}

			Transform.SetLocation(/*ShadowsInfo[i].Actor->GetActorLocation() + */ShadowsInfo[i].Offset + ShadowSocketPosition);
			Transform.SetRotation(FRotator(0, 0, 0).Quaternion());
			
			if (i == ShadowsInfo.Num() - 1)
			{
				IsDirty = true;
			}
			ISM_Comp->UpdateInstanceTransform(ShadowsInfo[i].Index, Transform, true, IsDirty);
		}
		else
		{
			//IndexesToHide.Add(i);

		}
	}

	// Hide invalid actors
	if (IndexesToHide.Num() > 0)
	{
		for (int32 Index : IndexesToHide)
		{
			//ISM_Comp->Instance
			//ShadowsInfo.RemoveAt(Index);

		}
	}
}

