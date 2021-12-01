// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellVFXComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "BaseSpellManager.h"
#include "UObject/ConstructorHelpers.h"
#include "HelperFunctions.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "SpellFunctionLibrary.h"

// Sets default values for this component's properties
USpellVFXComponent::USpellVFXComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	MainVFX = CreateDefaultSubobject<UNiagaraComponent>(FName(TEXT("MainVFX")));
	MainVFX->SetCanEverAffectNavigation(false);
	//MainVFX->Owner
	//MainNiagaraFX->SetupAttachment(RootComponent);

	SetVFXDataTable();
}


// Called when the game starts
void USpellVFXComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void USpellVFXComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	//MainVFX->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(SpellOwner->CurrentDirection), false);
}

void USpellVFXComponent::StartMainVFX()
{
	WakeUp();

	if (MainVFX)
	{
		MainVFX->Activate(true);
	}
}

void USpellVFXComponent::StopMainVFX()
{
	if (MainVFX)
	{
		MainVFX->DeactivateImmediate();
	}
}

void USpellVFXComponent::SpawnHitVFX(FVector Location)
{
	if (SpellManagerOwner == nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("SpellManagerOwner or SpellOwner is nullptr spellvfx component!!!")));
		return;
	}

	if (!HitVFX) HitVFX = GetNiagaraSystem(SpellFXType::ON_HIT);

	UWorld* World = GetWorld();
	if (!World) return;

	//TODO manual pool and measure perf
	if (HitVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, HitVFX, Location, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::AutoRelease, true);
	}

	// Print helper error message
	else
	{
		FString ss = "";
		ss += USpellFunctionLibrary::GetElementName(SpellManagerOwner->CurrentSpellInfo.Element);
		ss += USpellFunctionLibrary::GetCastTypeName(SpellManagerOwner->CurrentSpellInfo.CastType);
		ss += USpellFunctionLibrary::GetSpellFXTypeName(SpellFXType::ON_HIT);

		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Failed to spawn Niagara System %s"), *ss));
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Niagara System %s"), *ss);
	}

}

void USpellVFXComponent::SetupVFX(ABaseSpellManager* NewSpellManager)
{
	SpellManagerOwner = NewSpellManager;
}

void USpellVFXComponent::Hibernate()
{
	SetComponentTickEnabled(false);
	MainVFX->SetComponentTickEnabled(false);
	MainVFX->SetVisibility(false);
}

void USpellVFXComponent::WakeUp()
{
	if (!IsComponentTickEnabled())
	{
		SetComponentTickEnabled(true);
		if(MainVFX) MainVFX->SetComponentTickEnabled(true);
	}

	MainVFX->SetVisibility(true);
}

UNiagaraSystem* USpellVFXComponent::GetNiagaraSystem(SpellFXType SpellFXType)
{
	if (!SpellManagerOwner) return nullptr;

	

	if (!VFX_DataTable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Couldn't find Datatable for Spell VFX")));
		return nullptr;
	}

	UNiagaraSystem* NS = nullptr;

	for (auto it : VFX_DataTable->GetRowMap())
	{

		FSpellVFXInfo* SpellVFXInfo = (FSpellVFXInfo*)(it.Value);
		//FSpellVFXInfo* SpellVFXInfo = Cast<FSpellVFXInfo>(it.Value);
		if (SpellVFXInfo)
		{
			if (SpellVFXInfo->_Element == SpellManagerOwner->CurrentSpellInfo.Element && SpellVFXInfo->_CastType == SpellManagerOwner->CurrentSpellInfo.CastType)
			{
				if (SpellFXType == SpellFXType::ON_SPAWN)
				{
					NS = SpellVFXInfo->SpawnFX.LoadSynchronous();
					return NS;
				}
				else if (SpellFXType == SpellFXType::MAIN)
				{
					NS = SpellVFXInfo->MainFX.LoadSynchronous();
					return NS;
				}
				else if (SpellFXType == SpellFXType::ON_HIT)
				{
					NS = SpellVFXInfo->HitFX.LoadSynchronous();
					return NS;
				}
				else if (SpellFXType == SpellFXType::ON_HIT)
				{
					NS = SpellVFXInfo->ExplodeFX.LoadSynchronous();
					return NS;
				}
			}
		}
	}

	if (!NS)
	{

		FString sss = USpellFunctionLibrary::GetCastTypeName(SpellManagerOwner->CurrentSpellInfo.CastType);
		sss.Append(" + ");
		FString aa = USpellFunctionLibrary::GetElementName(SpellManagerOwner->CurrentSpellInfo.Element);
		sss.Append(aa);
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Failed to find Niagara System %s"), *sss));
		SET_WARN_COLOR(COLOR_RED);
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Niagara System %s"), *sss);
		CLEAR_WARN_COLOR();
		return nullptr;
	}
	return nullptr;
}

void USpellVFXComponent::SetVFXDataTable()
{
	ConstructorHelpers::FObjectFinder<UDataTable>DataTableAsset(TEXT("DataTable'/Game/GameSettings/DT_SpellVFX.DT_SpellVFX'"));
	UDataTable* DT = DataTableAsset.Object;
	if (DT)
	{
		VFX_DataTable = DT;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Yellow, TEXT("No VFX DataTable found!"));
	}
}

void USpellVFXComponent::GetVFXDataFromDT(UStaticMesh*& Mesh, UMaterialInterface*& Mat)
{
	if (!VFX_DataTable || !SpellManagerOwner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Couldn't find Datatable for Spell VFX")));
		return;
	}

	for (auto it : VFX_DataTable->GetRowMap())
	{
		FSpellVFXInfo* SpellVFXInfo = (FSpellVFXInfo*)(it.Value);
		//FSpellVFXInfo* SpellVFXInfo = Cast<FSpellVFXInfo>(it.Value);
		if (SpellVFXInfo)
		{
			if (SpellVFXInfo->_Element == SpellManagerOwner->CurrentSpellInfo.Element && SpellVFXInfo->_CastType == SpellManagerOwner->CurrentSpellInfo.CastType)
			{
				Mesh = SpellVFXInfo->MainMesh.LoadSynchronous();
				Mat = SpellVFXInfo->MainMaterial.LoadSynchronous();

				return;
			}
		}
	}
	return;

}