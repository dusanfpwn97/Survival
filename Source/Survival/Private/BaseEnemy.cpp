// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "BaseSpell.h"
#include "Components/CapsuleComponent.h"

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
	
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemy::OnCollidedWithSpell_Implementation(ABaseSpell* Spell)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("ssdsdasdw"));
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

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Mesh")));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionProfileName(FName(TEXT("NoCollision")));

	MainCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapBegin);
	MainCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapEnd);
}

void ABaseEnemy::Start()
{
	bIsActive = true;
	SetActorTickEnabled(true);
}

void ABaseEnemy::Reset()
{
	bIsActive = false;
	SetActorTickEnabled(false);
		
}

int32 ABaseEnemy::GetPoolIndex_Implementation()
{
	return PoolIndex;
}
