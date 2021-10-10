// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "BaseSpell.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupComponents();
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	temp = FMath::RandRange(-10.f, 10.f);
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveTowardsTarget();
	
}

void ABaseEnemy::OnCollidedWithSpell_Implementation(ABaseSpell* Spell)
{
	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("ssdsdasdw"));
}


void ABaseEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		if (OverlappedComp == MainCollider && OtherActor->Implements<UCombatInterface>())
		{

			ICombatInterface::Execute_OnCollidedWithEnemy(OtherActor, this);
		}
}

void ABaseEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABaseEnemy::SetupComponents()
{
	MainCollider = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("MainCollider")));
	RootComponent = MainCollider;
	MainCollider->SetGenerateOverlapEvents(true);
	MainCollider->SetCollisionProfileName(FName(TEXT("Enemy")));
	MainCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapBegin);
	MainCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapEnd);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Mesh")));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionProfileName(FName(TEXT("NoCollision")));

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("SkeletalMesh")));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));

}

void ABaseEnemy::Start_Implementation()
{
	bIsActive = true;
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	//GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ABaseEnemy::Reset_Implementation, 20.f/*FMath::RandRange(2.2f, 5.3f)*/, false);
}

void ABaseEnemy::Reset_Implementation()
{
	bIsActive = false;
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	if (Spawner)
	{
		IPoolInterface::Execute_ReleaseToPool(Spawner, this);
	}
	else GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spawner not valid! Should not happen. BaseEnemy.cpp -> Reset implementation"));
	
}

void ABaseEnemy::SetSpawner_Implementation(UObject* Object)
{
	if (!Object)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spawner not valid! Should not happen. BaseEnemy.cpp -> SetSpawner_Implementation"));
		return;
	}
	Spawner = Object;
}

void ABaseEnemy::SetTarget_Implementation(AActor* TargetActor)
{
	if (TargetActor)
	{
		Target = TargetActor;
	}
}

void ABaseEnemy::MoveTowardsTarget()
{
	UWorld* World = GetWorld();
	if (!World || !Target) return;

	FHitResult Hit;
	FVector Direction;
	Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Normalize();
	Direction.Z = 0.f;


	FHitResult Hits;
	FVector Start = GetActorLocation() - GetActorRightVector()*30;
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	TArray<AActor*> HitActors;
	


	
	LastRotation = FMath::RInterpTo(LastRotation, (Direction.Rotation() + FRotator(0.f, -90.f, 0.f)), World->GetDeltaSeconds(), 6.f);
	SetActorRotation(LastRotation, ETeleportType::None);


	for (int i = -2; i < 3; i++)
	{
		FRotator TempRot = (Direction.Rotation() + FRotator(0.f, i * -45.f, 0.f));
		FVector End = Start + TempRot.Vector() * MainCollider->GetUnscaledCapsuleRadius()*2.5;

		World->LineTraceSingleByObjectType(Hits, Start, End, ObjectQueryParams, Params);
		//DrawDebugLine(World, Start, End, FColor::Green, false, 0.f);
		if (Hits.IsValidBlockingHit())
		{
			HitActors.Add(Hits.GetActor());

			
		}
	}
	if (HitActors.Num() > 0)
	{
		for (AActor* TempActor : HitActors)
		{
			FVector Temp = TempActor->GetActorLocation() - GetActorLocation();
			Temp.Normalize();
			//Temp.
			Direction += -Temp * 0.85;
		}
		Direction.Normalize();
		Direction.Z = 0;
	}

	Velocity = FMath::VInterpTo(Velocity, Direction * 2.5, World->GetDeltaSeconds(), 6);
	//Velocity = Direction * 200 * World->GetDeltaSeconds();

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("%f"), Velocity.X));
	//Velocity += -Velocity * 0.4;

	AddActorWorldOffset(Velocity, false, &Hit, ETeleportType::None);





}

