// Copyright Project Ascendant. All Rights Reserved.

#include "UI/PAGameplayHUDWidget.h"
#include "UI/PAPlayerVitalsWidget.h"
#include "UI/PABossHealthWidget.h"
#include "UI/PAMerchantShopWidget.h"
#include "UI/PABlacksmithForgeWidget.h"
#include "UI/PAFloatingCombatTextComponent.h"
#include "Controller/PABasePlayerController.h"
#include "Character/PABaseCharacter.h"
#include "Combat/AscendantAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "Blueprint/UserWidget.h"

namespace
{
	template <typename T>
	T* CreateSafeWidget(UObject* Outer, TSubclassOf<T> Class, APlayerController* PC)
	{
		if (!Class)
		{
			return nullptr;
		}
		if (PC)
		{
			return CreateWidget<T>(PC, Class);
		}
		if (UWorld* World = Outer ? Outer->GetWorld() : nullptr)
		{
			return CreateWidget<T>(World, Class);
		}
		return NewObject<T>(Outer, Class);
	}
}

void UPAGameplayHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!VitalsWidgetClass)
	{
		VitalsWidgetClass = UPAPlayerVitalsWidget::StaticClass();
	}
	if (!BossHealthWidgetClass)
	{
		BossHealthWidgetClass = UPABossHealthWidget::StaticClass();
	}
	if (!ShopWidgetClass)
	{
		ShopWidgetClass = UPAMerchantShopWidget::StaticClass();
	}
	if (!ForgeWidgetClass)
	{
		ForgeWidgetClass = UPABlacksmithForgeWidget::StaticClass();
	}

	EnsureChildWidgetsCreated();
}

void UPAGameplayHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPAGameplayHUDWidget::NativeDestruct()
{
	CloseInteractiveWindows();
	Super::NativeDestruct();
}

void UPAGameplayHUDWidget::EnsureChildWidgetsCreated()
{
	APlayerController* PC = OwningPC.Get() ? OwningPC.Get() : GetOwningPlayer();

	if (!VitalsWidget && VitalsWidgetClass)
	{
		VitalsWidget = CreateSafeWidget<UPAPlayerVitalsWidget>(this, VitalsWidgetClass, PC);
		if (VitalsWidget)
		{
			VitalsWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (!BossHealthWidget && BossHealthWidgetClass)
	{
		BossHealthWidget = CreateSafeWidget<UPABossHealthWidget>(this, BossHealthWidgetClass, PC);
		if (BossHealthWidget)
		{
			BossHealthWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (!ShopWidget && ShopWidgetClass)
	{
		ShopWidget = CreateSafeWidget<UPAMerchantShopWidget>(this, ShopWidgetClass, PC);
		if (ShopWidget)
		{
			ShopWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (!ForgeWidget && ForgeWidgetClass)
	{
		ForgeWidget = CreateSafeWidget<UPABlacksmithForgeWidget>(this, ForgeWidgetClass, PC);
		if (ForgeWidget)
		{
			ForgeWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UPAGameplayHUDWidget::InitializeHUD(APABasePlayerController* InPC, APABaseCharacter* InCharacter)
{
	OwningPC = InPC;
	OwningCharacter = InCharacter;

	EnsureChildWidgetsCreated();

	if (VitalsWidget && InCharacter)
	{
		UAscendantAttributeSet* AttributeSet = InCharacter->GetAttributeSet();
		UAbilitySystemComponent* ASC = InCharacter->GetAbilitySystemComponent();

		if (AttributeSet && ASC)
		{
			VitalsWidget->BindToAttributeSet(AttributeSet, ASC);
		}
	}
}

void UPAGameplayHUDWidget::ShowBossHUD(AActor* BossActor, const FString& BossName, int32 BossLevel, const TArray<FName>& PartIds)
{
	EnsureChildWidgetsCreated();

	if (BossHealthWidget)
	{
		BossHealthWidget->SetBossTarget(BossActor, BossName, BossLevel, PartIds);
		BossHealthWidget->SetVisibility(ESlateVisibility::Visible);
		bIsBossHUDActive = true;
	}
}

void UPAGameplayHUDWidget::HideBossHUD()
{
	if (BossHealthWidget)
	{
		BossHealthWidget->ClearBossTarget();
		BossHealthWidget->SetVisibility(ESlateVisibility::Collapsed);
		bIsBossHUDActive = false;
	}
}

void UPAGameplayHUDWidget::SpawnCombatText(const FVector& WorldLocation, float Amount, EPACombatTextType Type)
{
	if (OwningPC.IsValid())
	{
		if (UPAFloatingCombatTextComponent* CombatTextComp = OwningPC->FindComponentByClass<UPAFloatingCombatTextComponent>())
		{
			CombatTextComp->SpawnCombatText(WorldLocation, Amount, Type);
		}
	}
}

void UPAGameplayHUDWidget::SpawnPerfectDodgeCallout(const FVector& CharacterLocation)
{
	if (OwningPC.IsValid())
	{
		if (UPAFloatingCombatTextComponent* CombatTextComp = OwningPC->FindComponentByClass<UPAFloatingCombatTextComponent>())
		{
			CombatTextComp->SpawnPerfectDodgeCallout(CharacterLocation);
		}
	}
}

void UPAGameplayHUDWidget::SetInteractionPrompt(const FString& PromptText, bool bVisible)
{
	CurrentPromptText = PromptText;
	bIsPromptVisible = bVisible;
}

void UPAGameplayHUDWidget::OpenShop(UPAMerchantComponent* MerchantComp, UPAInventoryComponent* PlayerInv,
	UPACurrencyComponent* PlayerWallet, int32 PlayerKarma, UPAServiceRequestComponent* RequestRouter)
{
	bIsShopOpen = true;
	bIsForgeOpen = false;

	if (ForgeWidget)
	{
		ForgeWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	EnsureChildWidgetsCreated();

	if (ShopWidget)
	{
		ShopWidget->InitializeShop(MerchantComp, PlayerInv, PlayerWallet, PlayerKarma, RequestRouter);
		ShopWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPAGameplayHUDWidget::OpenForge(UPABlacksmithComponent* ForgeComp, UPAInventoryComponent* PlayerInv,
	UPACurrencyComponent* PlayerWallet, UPAServiceRequestComponent* RequestRouter)
{
	bIsForgeOpen = true;
	bIsShopOpen = false;

	if (ShopWidget)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	EnsureChildWidgetsCreated();

	if (ForgeWidget)
	{
		ForgeWidget->InitializeForge(ForgeComp, PlayerInv, PlayerWallet, RequestRouter);
		ForgeWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPAGameplayHUDWidget::CloseInteractiveWindows()
{
	bIsShopOpen = false;
	bIsForgeOpen = false;

	if (ShopWidget)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ForgeWidget)
	{
		ForgeWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

bool UPAGameplayHUDWidget::IsAnyInteractiveWindowOpen() const
{
	return bIsShopOpen || bIsForgeOpen;
}
