// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellVFXComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "BaseSpell.h"
#include "BaseSpellManager.h"
#include "UObject/ConstructorHelpers.h"
#include "HelperFunctions.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
USpellVFXComponent::USpellVFXComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	MainVFX = CreateDefaultSubobject<UNiagaraComponent>(FName(TEXT("MainVFX")));
	
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



// Called every frame
void USpellVFXComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SpellOwner)
	{
		MainVFX->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(SpellOwner->LastDirection));
	}
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

void USpellVFXComponent::StartHitVFX()
{
	if (SpellManagerOwner == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("SpellManagerOwner or SpellOwner is nullptr!!!")));
		return;
	}

	if (!HitVFX) HitVFX = GetNiagaraSystem(SpellFXType::ON_HIT);

	UWorld* World = GetWorld();
	if (!World) return;

	if (HitVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, HitVFX, SpellOwner->GetActorLocation(), FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::AutoRelease, true);
	}

	// Print helper error message
	else
	{
		FString ss = "";
		ss += UHelperFunctions::GetElementName(SpellManagerOwner->CurrentSpellInfo.Element);
		ss += UHelperFunctions::GetCastTypeName(SpellManagerOwner->CurrentSpellInfo.CastType);
		ss += UHelperFunctions::GetSpellFXTypeName(SpellFXType::ON_HIT);

		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, FString::Printf(TEXT("Failed to spawn Niagara System %s"), *ss));
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Niagara System %s"), *ss);
	}

}

void USpellVFXComponent::SetupVFX(UBaseSpellManager* NewSpellManager, ABaseSpell* NewSpellOwner)
{
	SpellManagerOwner = NewSpellManager;
	SpellOwner = NewSpellOwner;

	if (MainVFX != nullptr)
	{
		FAttachmentTransformRules Rules = FAttachmentTransformRules::SnapToTargetIncludingScale;
		EAttachmentRule ARule = EAttachmentRule::SnapToTarget;
		Rules.RotationRule = ARule;
		MainVFX->AttachToComponent(NewSpellOwner->GetRootComponent(), Rules);

		MainVFX->SetAsset(GetNiagaraSystem(SpellFXType::MAIN));
	}

	
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
			if (SpellVFXInfo->Binding.Element == SpellManagerOwner->CurrentSpellInfo.Element && SpellVFXInfo->Binding.CastType == SpellManagerOwner->CurrentSpellInfo.CastType)
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
			}
		}
	}

	if (!NS)
	{

		FString sss = UHelperFunctions::GetCastTypeName(SpellManagerOwner->CurrentSpellInfo.CastType);
		sss.Append(" + ");
		FString aa = UHelperFunctions::GetElementName(SpellManagerOwner->CurrentSpellInfo.Element);
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
