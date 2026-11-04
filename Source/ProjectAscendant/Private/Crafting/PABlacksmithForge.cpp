// Copyright Project Ascendant. All Rights Reserved.

#include "Crafting/PABlacksmithForge.h"
#include "Crafting/PABlacksmithComponent.h"
#include "Components/StaticMeshComponent.h"

APABlacksmithForge::APABlacksmithForge()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ForgeMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ForgeMeshComp"));
	RootComponent = ForgeMeshComp;
	ForgeMeshComp->SetCollisionProfileName(TEXT("BlockAll"));

	BlacksmithComp = CreateDefaultSubobject<UPABlacksmithComponent>(TEXT("BlacksmithComponent"));
	ForgeTier = EPABlacksmithTier::Tier1_Outpost;
}

void APABlacksmithForge::BeginPlay()
{
	Super::BeginPlay();

	if (BlacksmithComp)
	{
		BlacksmithComp->SetForgeTier(ForgeTier);
	}
}
