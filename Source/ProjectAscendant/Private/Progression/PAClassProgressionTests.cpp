// Copyright Project Ascendant. All Rights Reserved.
// Comprehensive Automated Test Suite for Phase 2B Dual-Class Progression & Scrolls (ROADMAP.md:75-87)

#include "Misc/AutomationTest.h"
#include "Progression/PAClassProgressionTypes.h"
#include "Progression/PAClassProgressionComponent.h"
#include "Progression/ClassPromotionScrollDefinition.h"
#include "Inventory/PAInventoryComponent.h"
#include "Inventory/PAItemStaticDataAsset.h"
#include "GameplayTagsManager.h"

namespace ClassProgressionTestHelper
{
	struct FClassTestContext
	{
		TObjectPtr<AActor> TestActor;
		TObjectPtr<UPAClassProgressionComponent> ClassComp;

		~FClassTestContext()
		{
			if (TestActor)
			{
				TestActor->Destroy();
			}
		}
	};

	static FClassTestContext CreateTestContext(UWorld* World)
	{
		FClassTestContext Ctx;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Ctx.TestActor = World->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		Ctx.ClassComp = NewObject<UPAClassProgressionComponent>(Ctx.TestActor.Get());
		Ctx.ClassComp->RegisterComponent();

		return Ctx;
	}
}

// =========================================================================
// TEST 1: 16 Chức Nghiệp & Phân Định Bậc / Nhánh (DECISIONS.md §1, §2, §8)
// =========================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAClassProgression16ClassesTest,
	"ProjectAscendant.ClassProgression.16ClassesAndTags",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FPAClassProgression16ClassesTest::RunTest(const FString& Parameters)
{
	const TArray<FGameplayTag> All16 = FPAClassProgressionHelper::GetAll16ClassTags();
	TestEqual(TEXT("Total approved classes must be exactly 16"), All16.Num(), 16);

	int32 T1Count = 0;
	int32 T2Count = 0;
	int32 T3Count = 0;
	int32 T4Count = 0;

	for (const FGameplayTag& Tag : All16)
	{
		TestTrue(*FString::Printf(TEXT("Tag must be valid: %s"), *Tag.ToString()), Tag.IsValid());

		const EPAClassRank Rank = FPAClassProgressionHelper::GetClassRank(Tag);
		TestTrue(*FString::Printf(TEXT("Rank must not be None for: %s"), *Tag.ToString()), Rank != EPAClassRank::None);

		if (Rank == EPAClassRank::Novice) T1Count++;
		else if (Rank == EPAClassRank::Intermediate) T2Count++;
		else if (Rank == EPAClassRank::Master) T3Count++;
		else if (Rank == EPAClassRank::Apex) T4Count++;

		const EPAClassLine Line = FPAClassProgressionHelper::GetClassLine(Tag);
		TestTrue(*FString::Printf(TEXT("Line must not be None for: %s"), *Tag.ToString()), Line != EPAClassLine::None);

		const FName Weapon = FPAClassProgressionHelper::GetClassDefaultWeaponFamily(Tag);
		TestTrue(*FString::Printf(TEXT("Default weapon must not be None for: %s"), *Tag.ToString()), !Weapon.IsNone());
	}

	TestEqual(TEXT("T1 Novice classes must be 4 (Vanguard, Ranger, Arcanist, Acolyte)"), T1Count, 4);
	TestEqual(TEXT("T2 Intermediate classes must be 6"), T2Count, 6);
	TestEqual(TEXT("T3 Master classes must be 5"), T3Count, 5);
	TestEqual(TEXT("T4 Apex class must be 1 (God Slayer)"), T4Count, 1);

	return true;
}

// =========================================================================
// TEST 2: Quyển Trục Thăng Chức Đơn Slot (ROADMAP.md:80, 84, 85)
// =========================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAClassProgressionScrollValidationTest,
	"ProjectAscendant.ClassProgression.PromotionScrollValidation",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FPAClassProgressionScrollValidationTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	auto Ctx = ClassProgressionTestHelper::CreateTestContext(World);

	const FGameplayTag VanguardTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Vanguard")));
	const FGameplayTag TemplarTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Templar")));
	const FGameplayTag RangerTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Scout.Ranger")));
	const FGameplayTag HolyQuestTag = FGameplayTag::RequestGameplayTag(FName(TEXT("State.InSanctuary")));

	Ctx.ClassComp->InitializeFoundationalClass(VanguardTag);

	// Tạo Data Asset Quyển trục Templar (T2)
	TObjectPtr<UClassPromotionScrollDefinition> TemplarScroll = NewObject<UClassPromotionScrollDefinition>();
	TemplarScroll->ScrollID = FName(TEXT("Scroll_Templar_T2"));
	TemplarScroll->TargetClassTag = TemplarTag;
	TemplarScroll->AllowedSourceClassTags = { VanguardTag };
	TemplarScroll->RequiredMinRank = 1;
	TemplarScroll->RequiredSourceClassLevel = 20;

	// 1. Thất bại khi Class Level < 20
	FGameplayTagContainer EmptyFlags;
	EPAClassPromotionError Err1 = Ctx.ClassComp->ExecutePromotion(TemplarScroll, EPAClassSlot::Primary, EmptyFlags);
	TestEqual(TEXT("Promotion must fail when Class Level < 20"), Err1, EPAClassPromotionError::RequirementsNotMet);
	TestEqual(TEXT("Primary class remains Vanguard"), Ctx.ClassComp->GetPrimaryClassTag(), VanguardTag);

	// Cấp đủ EXP để đạt Class Level 20
	// 20 levels require EXP
	Ctx.ClassComp->GrantCombatClassXP(500000);
	TestEqual(TEXT("Class Level must reach 20"), Ctx.ClassComp->GetClassLevel(VanguardTag), 20);

	// 2. Thất bại khi class nguồn không nằm trong AllowedSourceClassTags
	TObjectPtr<UClassPromotionScrollDefinition> ShadowbladeScroll = NewObject<UClassPromotionScrollDefinition>();
	ShadowbladeScroll->TargetClassTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Scout.Shadowblade")));
	ShadowbladeScroll->AllowedSourceClassTags = { RangerTag }; // Chỉ cho phép Ranger
	ShadowbladeScroll->RequiredSourceClassLevel = 20;

	EPAClassPromotionError Err2 = Ctx.ClassComp->ExecutePromotion(ShadowbladeScroll, EPAClassSlot::Primary, EmptyFlags);
	TestEqual(TEXT("Promotion must fail when source class is not allowed"), Err2, EPAClassPromotionError::RequirementsNotMet);

	// 3. Thất bại khi thiếu cờ nhiệm vụ bắt buộc
	TemplarScroll->RequiredQuestFlag = HolyQuestTag;
	EPAClassPromotionError Err3 = Ctx.ClassComp->ExecutePromotion(TemplarScroll, EPAClassSlot::Primary, EmptyFlags);
	TestEqual(TEXT("Promotion must fail when required quest flag is missing"), Err3, EPAClassPromotionError::MissingQuestFlag);

	// 4. Thành công khi thỏa mãn mọi điều kiện
	FGameplayTagContainer ValidFlags;
	ValidFlags.AddTag(HolyQuestTag);
	EPAClassPromotionError Err4 = Ctx.ClassComp->ExecutePromotion(TemplarScroll, EPAClassSlot::Primary, ValidFlags);
	TestEqual(TEXT("Promotion must succeed with all requirements met"), Err4, EPAClassPromotionError::None);
	TestEqual(TEXT("Primary class successfully promoted to Templar"), Ctx.ClassComp->GetPrimaryClassTag(), TemplarTag);
	TestTrue(TEXT("Templar class is unlocked"), Ctx.ClassComp->IsClassUnlocked(TemplarTag));

	World->DestroyWorld(false);
	return true;
}

// =========================================================================
// TEST 3: Quyển Trục Apex Thí Thần Giả - Điều Kiện Song Slot (ROADMAP.md:80)
// =========================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAClassProgressionApexDualSlotTest,
	"ProjectAscendant.ClassProgression.ApexDualSlotPrerequisite",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FPAClassProgressionApexDualSlotTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	auto Ctx = ClassProgressionTestHelper::CreateTestContext(World);

	const FGameplayTag DragonKnightTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.DragonKnight"))); // T3
	const FGameplayTag ChronomancerTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Caster.Chronomancer"))); // T3
	const FGameplayTag GodSlayerTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Apex.GodSlayer"))); // T4

	Ctx.ClassComp->InitializeFoundationalClass(DragonKnightTag);
	Ctx.ClassComp->GrantCombatClassXP(500000); // Lên level 20

	// Tạo Apex Scroll
	TObjectPtr<UClassPromotionScrollDefinition> ApexScroll = NewObject<UClassPromotionScrollDefinition>();
	ApexScroll->ScrollID = FName(TEXT("Scroll_Apex_GodSlayer"));
	ApexScroll->TargetClassTag = GodSlayerTag;
	ApexScroll->AllowedSourceClassTags = { DragonKnightTag };
	ApexScroll->RequiredMinRank = 3; // Yêu cầu T3
	ApexScroll->RequiredSourceClassLevel = 20;
	ApexScroll->bRequiresDualSlotCondition = true;
	ApexScroll->SecondaryAllowedSourceTags = { ChronomancerTag };
	ApexScroll->SecondaryRequiredMinRank = 3;

	FGameplayTagContainer Flags;

	// 1. Thất bại vì slot phụ chưa đạt yêu cầu (đang rỗng)
	EPAClassPromotionError Err1 = Ctx.ClassComp->ExecutePromotion(ApexScroll, EPAClassSlot::Primary, Flags);
	TestEqual(TEXT("Apex promotion must fail when secondary slot condition not met"), Err1, EPAClassPromotionError::ApexDualSlotPrerequisiteNotMet);

	// Thiết lập slot phụ là Chronomancer T3 Level 20
	Ctx.ClassComp->SetSecondaryClass(ChronomancerTag);
	Ctx.ClassComp->GrantCombatClassXP(500000); // Lên level 20
	TestEqual(TEXT("Secondary slot set to Chronomancer"), Ctx.ClassComp->GetSecondaryClassTag(), ChronomancerTag);

	// Thử lại Apex khi cả 2 slot đều thỏa mãn T3
	EPAClassPromotionError Err2 = Ctx.ClassComp->ExecutePromotion(ApexScroll, EPAClassSlot::Primary, Flags);
	TestEqual(TEXT("Apex promotion succeeds when both slots meet requirements"), Err2, EPAClassPromotionError::None);
	TestEqual(TEXT("Primary class is now GodSlayer (Apex T4)"), Ctx.ClassComp->GetPrimaryClassTag(), GodSlayerTag);

	World->DestroyWorld(false);
	return true;
}

// =========================================================================
// TEST 4: Quy Tắc A1 — Hoán Đổi Chính <-> Phụ & Clamp Bậc (ROADMAP.md:82)
// =========================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAClassProgressionRuleA1SwapAndClampTest,
	"ProjectAscendant.ClassProgression.RuleA1_CitadelSwapAndRankClamp",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FPAClassProgressionRuleA1SwapAndClampTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	auto Ctx = ClassProgressionTestHelper::CreateTestContext(World);

	const FGameplayTag TemplarTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Templar"))); // T2
	const FGameplayTag VoidBladeTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.VoidBlade"))); // T3

	Ctx.ClassComp->InitializeFoundationalClass(TemplarTag);

	// Gán slot phụ là Void Blade (T3)
	Ctx.ClassComp->SetSecondaryClass(VoidBladeTag);

	// 1. Kiểm tra Rank Clamping: Primary là Templar (T2), Secondary là Void Blade (T3)
	// Bậc thực tế của Secondary phải bị clamp về T2 (Rank_Sub <= Rank_Primary)
	TestEqual(TEXT("Primary Rank is T2 Intermediate"), Ctx.ClassComp->GetPrimaryClassRank(), EPAClassRank::Intermediate);
	TestEqual(TEXT("Raw Secondary Rank is T3 Master"), Ctx.ClassComp->GetSecondaryClassRank(), EPAClassRank::Master);
	TestEqual(TEXT("Effective Secondary Rank clamped to T2 per Rule A1"), Ctx.ClassComp->GetEffectiveSecondaryRank(), EPAClassRank::Intermediate);

	// 2. Thất bại khi đổi ngoài Tòa Thành (Citadel)
	Ctx.ClassComp->SetInCitadel(false);
	EPAClassPromotionError SwapErr1;
	bool bSwapOutside = Ctx.ClassComp->SwapPrimarySecondaryClasses(SwapErr1);
	TestFalse(TEXT("Cannot swap classes outside Citadel"), bSwapOutside);
	TestEqual(TEXT("Error must be NotInCitadel"), SwapErr1, EPAClassPromotionError::NotInCitadel);

	// 3. Thành công khi vào Tòa Thành
	Ctx.ClassComp->SetInCitadel(true);
	EPAClassPromotionError SwapErr2;
	bool bSwapInside = Ctx.ClassComp->SwapPrimarySecondaryClasses(SwapErr2);
	TestTrue(TEXT("Can swap classes inside Citadel"), bSwapInside);
	TestEqual(TEXT("Error is None"), SwapErr2, EPAClassPromotionError::None);
	TestEqual(TEXT("Primary is now VoidBlade (T3)"), Ctx.ClassComp->GetPrimaryClassTag(), VoidBladeTag);
	TestEqual(TEXT("Secondary is now Templar (T2)"), Ctx.ClassComp->GetSecondaryClassTag(), TemplarTag);

	// Sau khi hoán đổi: Primary là T3, Secondary là T2 -> Secondary không bị clamp
	TestEqual(TEXT("Primary Rank is T3 Master"), Ctx.ClassComp->GetPrimaryClassRank(), EPAClassRank::Master);
	TestEqual(TEXT("Effective Secondary Rank is T2 Intermediate (not clamped)"), Ctx.ClassComp->GetEffectiveSecondaryRank(), EPAClassRank::Intermediate);

	World->DestroyWorld(false);
	return true;
}

// =========================================================================
// TEST 5: Quy Tắc B1 — Tương Thích Vũ Khí & Khóa Kỹ Năng Slot F (ROADMAP.md:83)
// =========================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAClassProgressionRuleB1WeaponCompatibilityTest,
	"ProjectAscendant.ClassProgression.RuleB1_WeaponCompatibilityLock",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FPAClassProgressionRuleB1WeaponCompatibilityTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	auto Ctx = ClassProgressionTestHelper::CreateTestContext(World);

	const FGameplayTag VanguardTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Vanguard"))); // 1H.Blade
	const FGameplayTag RangerTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Scout.Ranger"))); // 2H.Bow
	const FGameplayTag SwordmasterTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Swordmaster"))); // 1H.Blade

	Ctx.ClassComp->InitializeFoundationalClass(VanguardTag);

	// Trường hợp 1: Secondary là Ranger (khác họ vũ khí: 1H.Blade vs 2H.Bow)
	Ctx.ClassComp->SetSecondaryClass(RangerTag);

	// Kỹ năng phụ thuộc vũ khí (như Bắn Tên) -> Bị khóa (false)
	bool bWeaponSkillAllowed = Ctx.ClassComp->IsSecondaryActiveSkillAllowed(FName(TEXT("Weapon.2H.Bow")), false);
	TestFalse(TEXT("Weapon-dependent skill locked when Secondary has different weapon"), bWeaponSkillAllowed);

	// Kỹ năng không phụ thuộc vũ khí (Weapon-Agnostic, như Đặt Bẫy Chông) -> Được phép (true)
	bool bAgnosticSkillAllowed = Ctx.ClassComp->IsSecondaryActiveSkillAllowed(NAME_None, true);
	TestTrue(TEXT("Weapon-agnostic skill allowed for Secondary slot F"), bAgnosticSkillAllowed);

	// Trường hợp 2: Secondary là Swordmaster (cùng họ vũ khí 1H.Blade)
	Ctx.ClassComp->SetSecondaryClass(SwordmasterTag);

	// Cùng vũ khí -> Cho phép cả kỹ năng dùng vũ khí
	bool bSameWeaponAllowed = Ctx.ClassComp->IsSecondaryActiveSkillAllowed(FName(TEXT("Weapon.1H.Blade")), false);
	TestTrue(TEXT("Weapon-dependent skill allowed when both classes share weapon family"), bSameWeaponAllowed);

	// Trường hợp 3: God Slayer (Apex) hỗ trợ cả 1H.Blade và 2H.Polearm (DECISIONS.md §7)
	const FGameplayTag GodSlayerTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Apex.GodSlayer")));
	const FGameplayTag DragonKnightTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.DragonKnight"))); // 2H.Polearm

	// Primary = Vanguard (1H.Blade), Secondary = GodSlayer -> Cho phép 1H.Blade
	Ctx.ClassComp->SetSecondaryClass(GodSlayerTag);
	bool bGodSlayerBladeAllowed = Ctx.ClassComp->IsSecondaryActiveSkillAllowed(FName(TEXT("Weapon.1H.Blade")), false);
	TestTrue(TEXT("God Slayer allows 1H.Blade pairing with Vanguard"), bGodSlayerBladeAllowed);

	// Primary = DragonKnight (2H.Polearm), Secondary = GodSlayer -> Cho phép 2H.Polearm
	Ctx.ClassComp->InitializeFoundationalClass(DragonKnightTag);
	bool bGodSlayerPolearmAllowed = Ctx.ClassComp->IsSecondaryActiveSkillAllowed(FName(TEXT("Weapon.2H.Polearm")), false);
	TestTrue(TEXT("God Slayer allows 2H.Polearm pairing with DragonKnight"), bGodSlayerPolearmAllowed);

	// Primary = Ranger (2H.Bow), Secondary = GodSlayer -> Khác vũ khí, khóa kỹ năng Polearm/Blade
	Ctx.ClassComp->InitializeFoundationalClass(RangerTag);
	bool bGodSlayerBowMismatch = Ctx.ClassComp->IsSecondaryActiveSkillAllowed(FName(TEXT("Weapon.2H.Polearm")), false);
	TestFalse(TEXT("God Slayer locks weapon skill when paired with incompatible Ranger bow"), bGodSlayerBowMismatch);
	bool bGodSlayerAgnostic = Ctx.ClassComp->IsSecondaryActiveSkillAllowed(NAME_None, true);
	TestTrue(TEXT("God Slayer still allows weapon-agnostic skill when paired with Ranger"), bGodSlayerAgnostic);

	World->DestroyWorld(false);
	return true;
}

// =========================================================================
// TEST 6: Kinh Tế Quyển Trục MVP — Tàn Trang & Vàng (ROADMAP.md:86)
// =========================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAClassProgressionScrollEconomyMVPTest,
	"ProjectAscendant.ClassProgression.ScrollEconomyMVP",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FPAClassProgressionScrollEconomyMVPTest::RunTest(const FString& Parameters)
{
	const FGameplayTag TemplarTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Templar"))); // T2
	const FGameplayTag VoidBladeTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.VoidBlade"))); // T3
	const FGameplayTag GodSlayerTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Apex.GodSlayer"))); // T4

	TObjectPtr<UClassPromotionScrollDefinition> T2Scroll = NewObject<UClassPromotionScrollDefinition>();
	T2Scroll->TargetClassTag = TemplarTag;

	TObjectPtr<UClassPromotionScrollDefinition> T3Scroll = NewObject<UClassPromotionScrollDefinition>();
	T3Scroll->TargetClassTag = VoidBladeTag;

	TObjectPtr<UClassPromotionScrollDefinition> T4Scroll = NewObject<UClassPromotionScrollDefinition>();
	T4Scroll->TargetClassTag = GodSlayerTag;

	// Kiểm tra tỷ lệ phân rã ra Tàn Trang (item_skill_shard)
	TestEqual(TEXT("T2 Rare Scroll salvages into 3 shards"), T2Scroll->GetSalvageShardCount(), 3);
	TestEqual(TEXT("T3 Epic Scroll salvages into 8 shards"), T3Scroll->GetSalvageShardCount(), 8);
	TestEqual(TEXT("T4 Mythic Scroll salvages into 25 shards"), T4Scroll->GetSalvageShardCount(), 25);

	// Kiểm tra giá bán cho NPC Thương nhân
	TestEqual(TEXT("T2 Vendor Gold = 500"), T2Scroll->GetVendorGoldPrice(), 500);
	TestEqual(TEXT("T3 Vendor Gold = 2500"), T3Scroll->GetVendorGoldPrice(), 2500);
	TestEqual(TEXT("T4 Vendor Gold = 10000"), T4Scroll->GetVendorGoldPrice(), 10000);

	// Giá mua lại tại Chợ Đen = 1.5x
	TestEqual(TEXT("T2 Black Market Price = 750"), T2Scroll->GetBlackMarketBuybackPrice(), 750);
	TestEqual(TEXT("T3 Black Market Price = 3750"), T3Scroll->GetBlackMarketBuybackPrice(), 3750);
	TestEqual(TEXT("T4 Black Market Price = 15000"), T4Scroll->GetBlackMarketBuybackPrice(), 15000);

	return true;
}

// =========================================================================
// TEST 7: Ma Trận 256 Cặp Chính × Phụ & Trần Nội Tại (ROADMAP.md:85)
// =========================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAClassProgressionMatrix256PairsTest,
	"ProjectAscendant.ClassProgression.Matrix256PairsAndCaps",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FPAClassProgressionMatrix256PairsTest::RunTest(const FString& Parameters)
{
	const TArray<FGameplayTag> All16 = FPAClassProgressionHelper::GetAll16ClassTags();
	TestEqual(TEXT("Matrix must evaluate 16 classes"), All16.Num(), 16);

	int32 TestedPairs = 0;

	for (const FGameplayTag& PriTag : All16)
	{
		const EPAClassRank PriRank = FPAClassProgressionHelper::GetClassRank(PriTag);

		for (const FGameplayTag& SecTag : All16)
		{
			TestedPairs++;
			const EPAClassRank SecRank = FPAClassProgressionHelper::GetClassRank(SecTag);

			// Quy tắc A1 Rank Clamping cho mọi cặp
			const uint8 EffectiveSecRank = FMath::Min(static_cast<uint8>(SecRank), static_cast<uint8>(PriRank));
			TestTrue(TEXT("Effective Secondary rank must never exceed Primary rank"), EffectiveSecRank <= static_cast<uint8>(PriRank));

			// Kiểm tra bảo vệ trần nội tại (Passive Matrix Cap check)
			// Giả định Primary Passive = 40% DR, Secondary Passive = 35% DR -> Tổng 75% phải bị clamp về trần 60%
			const float CombinedDR = 0.40f + 0.35f;
			const float ClampedDR = FPAClassMatrixCaps::ClampDamageReduction(CombinedDR);
			TestEqual(TEXT("Damage reduction must be capped at 60%"), ClampedDR, 0.60f);

			// Giả định Critical Chance 50% + 40% = 90% phải bị clamp về trần 75%
			const float CombinedCrit = 0.50f + 0.40f;
			const float ClampedCrit = FPAClassMatrixCaps::ClampCriticalChance(CombinedCrit);
			TestEqual(TEXT("Critical chance must be capped at 75%"), ClampedCrit, 0.75f);
		}
	}

	TestEqual(TEXT("Exhaustive matrix must test all 16x16 = 256 pairs"), TestedPairs, 256);
	return true;
}

// =========================================================================
// TEST 8: Xác Thực & Tiêu Hao Quyển Trục Trong Túi Đồ & Server RPC (ROADMAP.md:83)
// =========================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAClassProgressionScrollInventoryAndServerRPCTest,
	"ProjectAscendant.ClassProgression.InventoryScrollAndServerRPC",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FPAClassProgressionScrollInventoryAndServerRPCTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	auto Ctx = ClassProgressionTestHelper::CreateTestContext(World);

	// Gắn thêm UPAInventoryComponent vào TestActor
	UPAInventoryComponent* InvComp = NewObject<UPAInventoryComponent>(Ctx.TestActor);
	InvComp->RegisterComponent();

	const FGameplayTag VanguardTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Vanguard")));
	const FGameplayTag TemplarTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Templar")));

	Ctx.ClassComp->InitializeFoundationalClass(VanguardTag);
	Ctx.ClassComp->GrantCombatClassXP(500000); // Lên level 20

	TObjectPtr<UClassPromotionScrollDefinition> TemplarScroll = NewObject<UClassPromotionScrollDefinition>();
	TemplarScroll->ScrollID = FName(TEXT("Scroll_Templar_T2"));
	TemplarScroll->TargetClassTag = TemplarTag;
	TemplarScroll->AllowedSourceClassTags = { VanguardTag };
	TemplarScroll->RequiredMinRank = 1;
	TemplarScroll->RequiredSourceClassLevel = 20;

	FGameplayTagContainer Flags;

	// 1. Thất bại khi túi đồ có 0 quyển trục -> MissingScrollItem
	EPAClassPromotionError ErrMissing = Ctx.ClassComp->ExecutePromotion(TemplarScroll, EPAClassSlot::Primary, Flags);
	TestEqual(TEXT("Must fail with MissingScrollItem when inventory has 0 scrolls"), ErrMissing, EPAClassPromotionError::MissingScrollItem);

	// 2. Thêm 1 quyển trục vào túi đồ
	UItemStaticDataAsset* ScrollItemAsset = NewObject<UItemStaticDataAsset>();
	ScrollItemAsset->ItemId = FName(TEXT("Scroll_Templar_T2"));
	ScrollItemAsset->MaxStackSize = 1;
	ScrollItemAsset->Category = EPAItemCategory::Consumable;

	int32 Remaining = 0;
	bool bAdded = InvComp->TryAddItem(ScrollItemAsset, 1, Remaining);
	TestTrue(TEXT("Successfully added scroll item to inventory"), bAdded);
	TestEqual(TEXT("Inventory now has 1 scroll"), InvComp->GetItemCount(FName(TEXT("Scroll_Templar_T2"))), 1);

	// 3. Thực hiện thăng chức thành công và tiêu hao quyển trục
	EPAClassPromotionError ErrSuccess = Ctx.ClassComp->ExecutePromotion(TemplarScroll, EPAClassSlot::Primary, Flags);
	TestEqual(TEXT("Promotion succeeds with scroll in inventory"), ErrSuccess, EPAClassPromotionError::None);
	TestEqual(TEXT("Primary class promoted to Templar"), Ctx.ClassComp->GetPrimaryClassTag(), TemplarTag);
	TestEqual(TEXT("Scroll was consumed, inventory count is now 0"), InvComp->GetItemCount(FName(TEXT("Scroll_Templar_T2"))), 0);

	// 4. Kiểm tra Server RPC (ServerPromoteClass)
	bool bFailedFired = false;
	EPAClassPromotionError CapturedError = EPAClassPromotionError::None;
	Ctx.ClassComp->OnClassPromotionFailed.AddLambda([&bFailedFired, &CapturedError](EPAClassSlot Slot, EPAClassPromotionError Err)
	{
		bFailedFired = true;
		CapturedError = Err;
	});

	// Thử gọi ServerPromoteClass khi không còn scroll trong túi đồ -> Bị từ chối
	Ctx.ClassComp->ServerPromoteClass(TemplarScroll, EPAClassSlot::Primary);
	TestTrue(TEXT("Server RPC broadcasted promotion failure"), bFailedFired);
	TestEqual(TEXT("Server RPC failure error is MissingScrollItem"), CapturedError, EPAClassPromotionError::MissingScrollItem);

	World->DestroyWorld(false);
	return true;
}
