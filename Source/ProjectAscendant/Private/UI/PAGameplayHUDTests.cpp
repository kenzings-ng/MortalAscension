// Copyright Project Ascendant. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "UI/PAGameplayHUDWidget.h"
#include "UI/PAPlayerVitalsWidget.h"
#include "UI/PABossHealthWidget.h"
#include "UI/PAMerchantShopWidget.h"
#include "UI/PABlacksmithForgeWidget.h"
#include "Crafting/PABlacksmithForge.h"
#include "Economy/PAWanderingSmuggler.h"
#include "Economy/PAMerchantComponent.h"
#include "Crafting/PABlacksmithComponent.h"
#include "Controller/PABasePlayerController.h"
#include "Character/PABaseCharacter.h"
#include "Inventory/PAInventoryComponent.h"
#include "Economy/PACurrencyComponent.h"
#include "Engine/World.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * FPAGameplayHUDTests
 *
 * Kiểm thử tự động cho Bước Z3 (In-Game HUD, NPC Placement & Service Request Interaction):
 *  - AC-1: Hierarchy & Initialization — Vitals, Boss HUD, Shop, Forge widgets.
 *  - AC-2: Boss HUD Lifecycle — Show/Hide Boss Encounter stats & Posture Reticle.
 *  - AC-3: Interactive Modal Windows — OpenShop, OpenForge, Close, and Mutual Exclusion.
 *  - AC-4: Proximity Interaction — Bán kính 300cm (DECISIONS.md §13), phím F router.
 *  - AC-5: Forge & Smuggler Actors — APABlacksmithForge & APAWanderingSmuggler component verification.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAGameplayHUDTests,
	"ProjectAscendant.UI.GameplayHUD",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPAGameplayHUDTests::RunTest(const FString& Parameters)
{
	// ===========================================================
	// AC-1: Hierarchy & Child Widget Instantiation
	// ===========================================================
	{
		UPAGameplayHUDWidget* HUDWidget = NewObject<UPAGameplayHUDWidget>();
		TestNotNull(TEXT("AC1: UPAGameplayHUDWidget created"), HUDWidget);

		if (HUDWidget)
		{
			// Gọi NativeConstruct để khởi tạo widget con
			HUDWidget->NativeConstruct();

			TestNotNull(TEXT("AC1: VitalsWidget instantiated"), HUDWidget->GetVitalsWidget());
			TestNotNull(TEXT("AC1: BossHealthWidget instantiated"), HUDWidget->GetBossHealthWidget());
			TestNotNull(TEXT("AC1: ShopWidget instantiated"), HUDWidget->GetShopWidget());
			TestNotNull(TEXT("AC1: ForgeWidget instantiated"), HUDWidget->GetForgeWidget());

			// Mặc định ban đầu: Vitals hiển thị, các cửa sổ modal bị đóng
			TestFalse(TEXT("AC1: Ban đầu không có cửa sổ tương tác nào mở"), HUDWidget->IsAnyInteractiveWindowOpen());
			TestFalse(TEXT("AC1: Boss HUD chưa kích hoạt khi chưa gặp boss"), HUDWidget->IsBossHUDVisible());
		}
	}

	// ===========================================================
	// AC-2: Boss HUD Lifecycle (Show / Hide Boss Stats)
	// ===========================================================
	{
		UPAGameplayHUDWidget* HUDWidget = NewObject<UPAGameplayHUDWidget>();
		if (HUDWidget)
		{
			HUDWidget->NativeConstruct();

			TArray<FName> PartIds = { FName("Horn"), FName("Tail") };
			HUDWidget->ShowBossHUD(nullptr, TEXT("Stone Golem"), 10, PartIds);

			TestTrue(TEXT("AC2: Boss HUD active sau khi ShowBossHUD"), HUDWidget->IsBossHUDVisible());
			if (UPABossHealthWidget* BossWidget = HUDWidget->GetBossHealthWidget())
			{
				TestEqual(TEXT("AC2: Tên Boss hiển thị đúng"), BossWidget->GetBossName(), TEXT("Stone Golem"));
				TestEqual(TEXT("AC2: Cấp độ Boss đúng"), BossWidget->GetBossLevel(), 10);
			}

			HUDWidget->HideBossHUD();
			TestFalse(TEXT("AC2: Boss HUD đóng sau khi HideBossHUD"), HUDWidget->IsBossHUDVisible());
		}
	}

	// ===========================================================
	// AC-3: Interactive Modal Windows (Shop & Forge)
	// ===========================================================
	{
		UPAGameplayHUDWidget* HUDWidget = NewObject<UPAGameplayHUDWidget>();
		if (HUDWidget)
		{
			HUDWidget->NativeConstruct();

			UPAMerchantComponent* MerchantComp = NewObject<UPAMerchantComponent>();
			UPABlacksmithComponent* ForgeComp = NewObject<UPABlacksmithComponent>();
			UPAInventoryComponent* InvComp = NewObject<UPAInventoryComponent>();
			UPACurrencyComponent* WalletComp = NewObject<UPACurrencyComponent>();

			// 1. Mở Cửa Hàng
			HUDWidget->OpenShop(MerchantComp, InvComp, WalletComp, 0, nullptr);
			TestTrue(TEXT("AC3: Shop mở thành công"), HUDWidget->IsAnyInteractiveWindowOpen());

			// 2. Chuyển sang Mở Lò Rèn -> Shop phải tự đóng (Mutual exclusion)
			HUDWidget->OpenForge(ForgeComp, InvComp, WalletComp, nullptr);
			TestTrue(TEXT("AC3: Forge mở thành công"), HUDWidget->IsAnyInteractiveWindowOpen());

			// 3. Đóng cửa sổ tương tác
			HUDWidget->CloseInteractiveWindows();
			TestFalse(TEXT("AC3: Cửa sổ tương tác đã đóng hoàn toàn"), HUDWidget->IsAnyInteractiveWindowOpen());
		}
	}

	// ===========================================================
	// AC-4: Interaction Prompt (300cm Radius)
	// ===========================================================
	{
		UPAGameplayHUDWidget* HUDWidget = NewObject<UPAGameplayHUDWidget>();
		if (HUDWidget)
		{
			HUDWidget->NativeConstruct();

			HUDWidget->SetInteractionPrompt(TEXT("[F] Giao Dịch"), true);
			TestTrue(TEXT("AC4: Prompt visible"), HUDWidget->IsInteractionPromptVisible());
			TestEqual(TEXT("AC4: Prompt text matches"), HUDWidget->GetInteractionPromptText(), TEXT("[F] Giao Dịch"));

			HUDWidget->SetInteractionPrompt(TEXT(""), false);
			TestFalse(TEXT("AC4: Prompt hidden"), HUDWidget->IsInteractionPromptVisible());
		}
	}

	// ===========================================================
	// AC-5: APABlacksmithForge & APAWanderingSmuggler Actor Verification
	// ===========================================================
	{
		// 1. Lò rèn Outpost Tier 1
		APABlacksmithForge* ForgeActor = NewObject<APABlacksmithForge>();
		TestNotNull(TEXT("AC5: APABlacksmithForge instantiated"), ForgeActor);
		if (ForgeActor)
		{
			TestEqual(TEXT("AC5: Mặc định Forge Tier là Tier 1 Outpost"), ForgeActor->ForgeTier, EPABlacksmithTier::Tier1_Outpost);
			TestEqual(TEXT("AC5: Bán kính tương tác Lò rèn là 300cm (DECISIONS.md §13)"), APABlacksmithForge::kMaxInteractionDistance, 300.0f);
			TestNotNull(TEXT("AC5: BlacksmithComponent tồn tại trên Forge Actor"), ForgeActor->GetBlacksmithComponent());
		}

		// 2. Thương Nhân Lang Thang
		APAWanderingSmuggler* SmugglerActor = NewObject<APAWanderingSmuggler>();
		TestNotNull(TEXT("AC5: APAWanderingSmuggler instantiated"), SmugglerActor);
		if (SmugglerActor)
		{
			TestNotNull(TEXT("AC5: MerchantComponent tồn tại trên Smuggler Actor"), SmugglerActor->GetMerchantComponent());
			TestNotNull(TEXT("AC5: RestockComponent tồn tại trên Smuggler Actor"), SmugglerActor->GetRestockComponent());
		}
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
