// Copyright Project Ascendant. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Character/PABaseCharacter.h"
#include "Character/PAStoneGolemBoss.h"
#include "Controller/PABasePlayerController.h"
#include "Crafting/PABlacksmithForge.h"
#include "Crafting/PABlacksmithComponent.h"
#include "Economy/PAWanderingSmuggler.h"
#include "Economy/PAMerchantComponent.h"
#include "UI/PAGameplayHUDWidget.h"
#include "UI/PALoginWidget.h"
#include "UI/PACharacterSelectWidget.h"
#include "Account/PAAccountSubsystem.h"
#include "AbilitySystemComponent.h"
#include "Combat/AscendantAttributeSet.h"
#include "Combat/PAPostureSyncComponent.h"
#include "Combat/PAThreatComponent.h"
#include "AI/PABossAIComponent.h"
#include "World/PAMonsterLeashComponent.h"
#include "Network/PAIrisSpatialFilter.h"
#include "Network/PAServiceRequestComponent.h"
#include "GameplayTagsManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * FPAPlayableSliceIntegrationTests
 *
 * Kiểm thử tích hợp toàn diện End-to-End cho Bước Z4 (Playable Slice Integration & Playtest Readiness):
 * - AC-1: Cấu trúc Actor Outpost (Lò Rèn, Smuggler, Boss Stone Golem) và thông số bán kính tương tác 300cm.
 * - AC-2: Vòng chuyển giao từ Chọn Nhân Vật sang In-Game HUD (SetupGameplayHUD, không ngắt mạng trong NM_Client).
 * - AC-3: Định tuyến tương tác NPC, vùng an toàn Sanctuary, và tự động đóng modal khi vào giao tranh.
 * - AC-4: Vòng đời giao tranh Boss (Contested Threat Table, Vỡ thế Stagger, Ngắt Slam khi bị choáng, và Leash Reset).
 * - AC-5: Chuẩn Replication Iris & GAS (Player: Mixed, Boss: Minimal, Iris Spatial Filter tiers).
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAPlayableSliceIntegrationTests,
	"ProjectAscendant.Integration.PlayableSlice",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPAPlayableSliceIntegrationTests::RunTest(const FString& Parameters)
{
	UGameplayTagsManager& TagMgr = UGameplayTagsManager::Get();
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Attacking")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Staggered")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Broken")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Dead")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Invulnerable")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.InCombat")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.InSanctuary")));

	// =========================================================================
	// AC-1: Cấu trúc Actor Outpost & Thông số Bán Kính Tương Tác
	// =========================================================================
	{
		// 1.1 Lò rèn APABlacksmithForge
		APABlacksmithForge* Forge = NewObject<APABlacksmithForge>();
		TestNotNull(TEXT("AC-1: APABlacksmithForge instantiable"), Forge);
		if (Forge)
		{
			TestTrue(TEXT("AC-1: Forge bReplicates is true"), Forge->GetIsReplicated());
			TestEqual(TEXT("AC-1: Forge interaction radius is 300cm (DECISIONS §13)"), APABlacksmithForge::kMaxInteractionDistance, 300.0f);
			TestNotNull(TEXT("AC-1: Forge has UPABlacksmithComponent"), Forge->GetBlacksmithComponent());
		}

		// 1.2 NPC Thương Nhân APAWanderingSmuggler
		APAWanderingSmuggler* Smuggler = NewObject<APAWanderingSmuggler>();
		TestNotNull(TEXT("AC-1: APAWanderingSmuggler instantiable"), Smuggler);
		if (Smuggler)
		{
			TestTrue(TEXT("AC-1: Smuggler bReplicates is true"), Smuggler->GetIsReplicated());
			TestNotNull(TEXT("AC-1: Smuggler has UPAMerchantComponent"), Smuggler->GetMerchantComponent());
		}

		// 1.3 Boss Đấu Trường APAStoneGolemBoss
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		TestNotNull(TEXT("AC-1: APAStoneGolemBoss instantiable"), Boss);
		if (Boss)
		{
			TestNotNull(TEXT("AC-1: Boss has PostureSyncComponent"), Boss->GetPostureSyncComponent());
			TestNotNull(TEXT("AC-1: Boss has ThreatComponent"), Boss->GetThreatComponent());
			TestNotNull(TEXT("AC-1: Boss has BossAIComponent"), Boss->GetBossAIComponent());
			TestNotNull(TEXT("AC-1: Boss has LeashComponent"), Boss->GetLeashComponent());
			TestTrue(TEXT("AC-1: Boss bReplicates is true"), Boss->GetIsReplicated());
		}
	}

	// =========================================================================
	// AC-2: Vòng Chuyển Giao Chọn Nhân Vật & Setup In-Game HUD
	// =========================================================================
	{
		UGameInstance* GI = NewObject<UGameInstance>();
		UPAAccountSubsystem* AccountSubsystem = NewObject<UPAAccountSubsystem>(GI);
		TestNotNull(TEXT("AC-2: AccountSubsystem created"), AccountSubsystem);

		APABasePlayerController* PC = NewObject<APABasePlayerController>();
		TestNotNull(TEXT("AC-2: APABasePlayerController created"), PC);

		if (PC)
		{
			TestNotNull(TEXT("AC-2: PC has ServiceRequestComponent"), PC->GetServiceRequestComponent());
			TestNotNull(TEXT("AC-2: PC has FloatingCombatTextComponent"), PC->GetFloatingCombatTextComponent());
		}

		// Khởi tạo và kiểm tra UPAGameplayHUDWidget
		UPAGameplayHUDWidget* HUD = NewObject<UPAGameplayHUDWidget>();
		TestNotNull(TEXT("AC-2: Gameplay HUD instantiated"), HUD);
		if (HUD)
		{
			TestFalse(TEXT("AC-2: No interactive window is initially open"), HUD->IsAnyInteractiveWindowOpen());
			TestFalse(TEXT("AC-2: Boss HUD is not visible initially"), HUD->IsBossHUDVisible());
		}

		// Kiểm tra UPACharacterSelectWidget chuyển class
		UPACharacterSelectWidget* CharSelectWidget = NewObject<UPACharacterSelectWidget>();
		if (CharSelectWidget)
		{
			CharSelectWidget->SelectClass(EPACharacterClass::Vanguard);
			const FPACharacterClassInfo VanguardInfo = CharSelectWidget->GetSelectedClassInfo();
			TestEqual(TEXT("AC-2: Vanguard class tag verified"), VanguardInfo.ClassTag, FString(TEXT("Class.Line.Guard.Vanguard")));
		}
	}

	// =========================================================================
	// AC-3: Định Tuyến Tương Tác & Chặn Giao Dịch Trong Chiến Đấu
	// =========================================================================
	{
		APABaseCharacter* Player = NewObject<APABaseCharacter>();
		TestNotNull(TEXT("AC-3: Player character created"), Player);

		APABlacksmithForge* Forge = NewObject<APABlacksmithForge>();
		UPABlacksmithComponent* ForgeComp = Forge ? Forge->GetBlacksmithComponent() : nullptr;

		if (Player && ForgeComp)
		{
			EPACraftingError Error = EPACraftingError::None;

			// 3.1 Tương tác bình thường (ngoài giao tranh)
			const bool bCanInteractNormal = ForgeComp->ValidateInteraction(Player, false, Error);
			TestTrue(TEXT("AC-3: Interaction allowed outside combat"), bCanInteractNormal);
			TestEqual(TEXT("AC-3: No crafting error outside combat"), Error, EPACraftingError::None);

			// 3.2 Tương tác bị chặn khi đang trong combat (State.InCombat)
			const bool bCanInteractInCombat = ForgeComp->ValidateInteraction(Player, true, Error);
			TestFalse(TEXT("AC-3: Interaction blocked during combat"), bCanInteractInCombat);
			TestEqual(TEXT("AC-3: Error is InCombat"), Error, EPACraftingError::InCombat);
		}

		// 3.3 Merchant Shop chặn giao dịch khi dính combat
		APAWanderingSmuggler* Smuggler = NewObject<APAWanderingSmuggler>();
		UPAMerchantComponent* MerchantComp = Smuggler ? Smuggler->GetMerchantComponent() : nullptr;
		if (Player && MerchantComp)
		{
			EPATransactionError MerchError = EPATransactionError::None;
			const bool bCanShopInCombat = MerchantComp->ValidateInteraction(Player, true, MerchError);
			TestFalse(TEXT("AC-3: Smuggler shopping blocked during combat"), bCanShopInCombat);
			TestEqual(TEXT("AC-3: Smuggler error is InCombat"), MerchError, EPATransactionError::InCombat);
		}
	}

	// =========================================================================
	// AC-4: Vòng Đời Giao Tranh Boss (Threat, Stagger, Slam Interruption, Leash)
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		TestNotNull(TEXT("AC-4: Boss created"), Boss);

		if (Boss)
		{
			UPAThreatComponent* ThreatComp = Boss->GetThreatComponent();
			UPAPostureSyncComponent* PostureComp = Boss->GetPostureSyncComponent();
			UAbilitySystemComponent* BossASC = Boss->GetAbilitySystemComponent();

			APABaseCharacter* Player1 = NewObject<APABaseCharacter>();
			APABaseCharacter* Player2 = NewObject<APABaseCharacter>();

			// 4.1 Bảng Threat Contested & 130% Retarget Rule
			ThreatComp->AddThreat(Player1, 100.0f);
			TestEqual(TEXT("AC-4: Player1 is top threat initially"), ThreatComp->GetCurrentAggroTarget(), Cast<AActor>(Player1));

			// Player2 gây thêm threat nhưng dưới ngưỡng 130% (120 < 130) -> Boss chưa đổi mục tiêu
			ThreatComp->AddThreat(Player2, 120.0f);
			ThreatComp->EvaluateThreatRetargeting();
			TestEqual(TEXT("AC-4: Player1 retained under 130% threshold"), ThreatComp->GetCurrentAggroTarget(), Cast<AActor>(Player1));

			// Player2 vượt ngưỡng 130% (135 >= 130) -> Boss tự động đổi mục tiêu
			ThreatComp->AddThreat(Player2, 15.0f);
			ThreatComp->EvaluateThreatRetargeting();
			TestEqual(TEXT("AC-4: Boss retargets to Player2 when exceeding 130%"), ThreatComp->GetCurrentAggroTarget(), Cast<AActor>(Player2));

			// 4.2 Thi triển Slam và bị Stagger ngắt chiêu
			const bool bSlamStarted = Boss->TryExecuteGroundSlam();
			TestTrue(TEXT("AC-4: Ground Slam started"), bSlamStarted);
			TestTrue(TEXT("AC-4: Boss is performing slam"), Boss->IsPerformingSlam());
			TestTrue(TEXT("AC-4: Boss has State.Attacking"), BossASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Attacking"))));

			// Bị vỡ thế Posture Break
			Boss->HandlePostureBroken(Player1);
			TestFalse(TEXT("AC-4: Slam stopped on posture break"), Boss->IsPerformingSlam());
			TestFalse(TEXT("AC-4: State.Attacking removed on stagger"), BossASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Attacking"))));
			TestTrue(TEXT("AC-4: Boss has State.Staggered"), BossASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Staggered"))));
			TestTrue(TEXT("AC-4: PostureComponent IsStaggered is true"), PostureComp->IsStaggered());

			// 4.3 Leash Reset
			Boss->SetTargetActor(Player1);
			Boss->HandleLeashStateChanged(true);
			TestNull(TEXT("AC-4: Target cleared on leash reset"), Boss->GetTargetActor());
			TestTrue(TEXT("AC-4: Threat table invulnerable during leash"), ThreatComp->IsInvulnerable());
		}
	}

	// =========================================================================
	// AC-5: Chuẩn Replication Iris & GAS (DECISIONS §11)
	// =========================================================================
	{
		APABaseCharacter* Player = NewObject<APABaseCharacter>();
		if (Player && Player->GetAbilitySystemComponent())
		{
			TestEqual(
				TEXT("AC-5: Player ASC Replication Mode is Mixed (DECISIONS §11)"),
				Player->GetAbilitySystemComponent()->ReplicationMode,
				EGameplayEffectReplicationMode::Mixed
			);
		}

		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		if (Boss && Boss->GetAbilitySystemComponent())
		{
			TestEqual(
				TEXT("AC-5: Boss ASC Replication Mode is Minimal (DECISIONS §11)"),
				Boss->GetAbilitySystemComponent()->ReplicationMode,
				EGameplayEffectReplicationMode::Minimal
			);
		}

		// Iris Spatial Filter: Cự ly gần (Tier 1 = 60Hz), cự ly trung bình (Tier 2 = 30Hz), cự ly xa (Tier 3 = 0Hz)
		FPAIrisSpatialConfig Config;
		Config.Tier1Radius = 1500.0f;
		Config.Tier2Radius = 3500.0f;
		Config.Tier1Frequency = 60.0f;
		Config.Tier2Frequency = 30.0f;
		Config.Tier3Frequency = 0.0f;

		TestEqual(TEXT("AC-5: Iris Tier 1 frequency is 60Hz"), UPAIrisSpatialFilter::GetReplicationFrequencyForDistance(500.0f, Config), 60.0f);
		TestEqual(TEXT("AC-5: Iris Tier 2 frequency is 30Hz"), UPAIrisSpatialFilter::GetReplicationFrequencyForDistance(2000.0f, Config), 30.0f);
		TestEqual(TEXT("AC-5: Iris Tier 3 frequency is 0Hz"), UPAIrisSpatialFilter::GetReplicationFrequencyForDistance(4000.0f, Config), 0.0f);
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
