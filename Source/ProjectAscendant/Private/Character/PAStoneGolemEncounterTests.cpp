// Copyright Project Ascendant. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Character/PAStoneGolemBoss.h"
#include "Character/PABaseCharacter.h"
#include "Combat/PAPostureSyncComponent.h"
#include "Combat/PAThreatComponent.h"
#include "AI/PABossAIComponent.h"
#include "World/PAMonsterLeashComponent.h"
#include "AbilitySystemComponent.h"
#include "Combat/AscendantAttributeSet.h"
#include "GameplayTagsManager.h"
#include "Engine/World.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * FPAStoneGolemEncounterTests
 *
 * Kiểm thử tích hợp cho bước Z3b (Playable Slice Boss Encounter & Wiring):
 * - AC-1: Khởi tạo đầy đủ các Subobject Component (PostureSyncComponent, ThreatComponent, BossAIComponent, LeashComponent).
 * - AC-2: Hệ thống Threat Table tích hợp (AddThreat, theo dõi mục tiêu Aggro Target).
 * - AC-3: Vòng lặp Vỡ thế Stagger & Cửa sổ kết liễu Finisher (HandlePostureBroken, GameplayTag State.Staggered, Finisher window).
 * - AC-4: Kỹ năng Ground Slam & Trạng thái giao tranh (TryExecuteGroundSlam, State.Attacking, Cooldown).
 * - AC-5: Cơ chế Leash Reset và xóa Nộ khí khi Boss quay đầu.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPAStoneGolemEncounterTests,
	"ProjectAscendant.Character.StoneGolemEncounter",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPAStoneGolemEncounterTests::RunTest(const FString& Parameters)
{
	UGameplayTagsManager& TagMgr = UGameplayTagsManager::Get();
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Attacking")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Staggered")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Broken")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Dead")));
	TagMgr.AddNativeGameplayTag(FName(TEXT("State.Invulnerable")));

	// =========================================================================
	// AC-1: Subobject Components Initialization
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		TestNotNull(TEXT("AC-1: APAStoneGolemBoss instantiated successfully"), Boss);

		TestNotNull(TEXT("AC-1: LeashComponent exists"), Boss->GetLeashComponent());
		TestNotNull(TEXT("AC-1: PostureSyncComponent exists"), Boss->GetPostureSyncComponent());
		TestNotNull(TEXT("AC-1: ThreatComponent exists"), Boss->GetThreatComponent());
		TestNotNull(TEXT("AC-1: BossAIComponent exists"), Boss->GetBossAIComponent());
		TestNotNull(TEXT("AC-1: AbilitySystemComponent exists"), Boss->GetAbilitySystemComponent());

		// Kiểm tra GAS replication mode là Minimal cho Boss theo DECISIONS §11
		TestEqual(
			TEXT("AC-1: Boss GAS Replication Mode is Minimal"),
			Boss->GetAbilitySystemComponent()->ReplicationMode,
			EGameplayEffectReplicationMode::Minimal
		);
	}

	// =========================================================================
	// AC-2: Threat Table & Target Tracking
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		UPAThreatComponent* ThreatComp = Boss->GetThreatComponent();
		TestNotNull(TEXT("AC-2: ThreatComponent valid"), ThreatComp);

		// Giả lập người chơi gây sát thương
		APABaseCharacter* PlayerA = NewObject<APABaseCharacter>();
		TestNotNull(TEXT("AC-2: PlayerA valid"), PlayerA);

		ThreatComp->AddThreat(PlayerA, 150.0f, EPAThreatSource::Damage);
		TestEqual(TEXT("AC-2: PlayerA threat score is 150"), ThreatComp->GetThreat(PlayerA), 150.0f);

		// Người chơi B khiêu khích (Taunt)
		APABaseCharacter* PlayerB = NewObject<APABaseCharacter>();
		TestNotNull(TEXT("AC-2: PlayerB valid"), PlayerB);

		ThreatComp->ApplyTaunt(PlayerB, 50.0f);
		// Taunt sẽ snap lên Max(Top + 100, Caster) = 150 + 100 = 250
		TestTrue(TEXT("AC-2: PlayerB taunt snaps above top threat"), ThreatComp->GetThreat(PlayerB) >= 250.0f);

		ThreatComp->EvaluateThreatRetargeting();
		TestEqual(TEXT("AC-2: PlayerB is current aggro target"), ThreatComp->GetCurrentAggroTarget(), Cast<AActor>(PlayerB));
	}

	// =========================================================================
	// AC-3: Stagger Loop & Finisher Window
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		UPAPostureSyncComponent* PostureSync = Boss->GetPostureSyncComponent();
		TestNotNull(TEXT("AC-3: PostureSyncComponent valid"), PostureSync);

		APABaseCharacter* VanguardPlayer = NewObject<APABaseCharacter>();
		TestNotNull(TEXT("AC-3: VanguardPlayer valid"), VanguardPlayer);

		// Boss chưa bị vỡ thế
		TestFalse(TEXT("AC-3: Boss initially not staggered"), PostureSync->IsStaggered());

		// Phá vỡ thế đứng Posture của Boss
		Boss->HandlePostureBroken(VanguardPlayer);

		// Xác thực cờ Stagger và tag State.Staggered
		TestTrue(TEXT("AC-3: PostureSyncComponent marks Boss staggered"), PostureSync->IsStaggered());

		UAbilitySystemComponent* BossASC = Boss->GetAbilitySystemComponent();
		TestNotNull(TEXT("AC-3: BossASC valid"), BossASC);
		TestTrue(
			TEXT("AC-3: Boss ASC has State.Staggered tag"),
			BossASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Staggered"), false))
		);

		// Xác thực VanguardPlayer có thể kích hoạt Finisher trong cửa sổ độc quyền
		TestTrue(
			TEXT("AC-3: Vanguard can execute finisher during exclusive window"),
			PostureSync->CanExecute(VanguardPlayer)
		);
	}

	// =========================================================================
	// AC-4: Ground Slam Combat & Animation Tag State
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();

		TestFalse(TEXT("AC-4: Not performing slam initially"), Boss->IsPerformingSlam());

		// Thi triển Ground Slam
		bool bSlamTriggered = Boss->TryExecuteGroundSlam();
		TestTrue(TEXT("AC-4: Ground slam executed"), bSlamTriggered);
		TestTrue(TEXT("AC-4: Boss is performing slam"), Boss->IsPerformingSlam());

		UAbilitySystemComponent* BossASC = Boss->GetAbilitySystemComponent();
		TestTrue(
			TEXT("AC-4: Boss has State.Attacking tag during slam"),
			BossASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Attacking"), false))
		);

		// Kết thúc chiêu Slam
		Boss->FinishGroundSlam();
		TestFalse(TEXT("AC-4: Not performing slam after finish"), Boss->IsPerformingSlam());
		TestFalse(
			TEXT("AC-4: State.Attacking tag removed after finish"),
			BossASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Attacking"), false))
		);
	}

	// =========================================================================
	// AC-5: Leash Reset & Threat Wipe
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		UPAThreatComponent* ThreatComp = Boss->GetThreatComponent();

		APABaseCharacter* TargetPlayer = NewObject<APABaseCharacter>();
		ThreatComp->AddThreat(TargetPlayer, 200.0f);
		Boss->SetTargetActor(TargetPlayer);

		TestNotNull(TEXT("AC-5: Target valid before leash"), Boss->GetTargetActor());

		// Boss vượt quá LeashBoundary (2500cm) và kích hoạt quay đầu
		UPAMonsterLeashComponent* Leash = Boss->GetLeashComponent();
		TestNotNull(TEXT("AC-5: Leash valid"), Leash);

		// Kích hoạt xử lý Leash Reset
		Boss->HandleLeashStateChanged(true);

		TestNull(TEXT("AC-5: Target cleared on leash reset"), Boss->GetTargetActor());
		TestTrue(TEXT("AC-5: ThreatComponent is in leash reset state"), ThreatComp->IsInvulnerable());
	}

	// =========================================================================
	// AC-6: Ground Slam Cancellation & Timer Protection
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		Boss->TryExecuteGroundSlam();
		TestTrue(TEXT("AC-6: Ground slam in progress"), Boss->IsPerformingSlam());

		// Boss bị đánh vỡ thế (Stagger) giữa chừng khi đang dập đất
		APABaseCharacter* Attacker = NewObject<APABaseCharacter>();
		Boss->HandlePostureBroken(Attacker);

		TestFalse(TEXT("AC-6: Ground slam cancelled immediately on Stagger"), Boss->IsPerformingSlam());
		TestFalse(
			TEXT("AC-6: State.Attacking tag removed on Stagger cancellation"),
			Boss->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Attacking"), false))
		);
	}

	// =========================================================================
	// AC-7: Threat Table RemoveCombatant & Stale Target Filtering
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		UPAThreatComponent* ThreatComp = Boss->GetThreatComponent();

		APABaseCharacter* Player1 = NewObject<APABaseCharacter>();
		APABaseCharacter* Player2 = NewObject<APABaseCharacter>();

		ThreatComp->AddThreat(Player1, 100.0f);
		ThreatComp->AddThreat(Player2, 80.0f);
		ThreatComp->EvaluateThreatRetargeting();

		TestEqual(TEXT("AC-7: Player1 is top aggro target initially"), ThreatComp->GetCurrentAggroTarget(), Cast<AActor>(Player1));

		// Giả lập Player1 bị loại bỏ (chết hoặc vào Sanctuary)
		ThreatComp->RemoveCombatant(Player1);
		TestEqual(TEXT("AC-7: Target automatically switched to Player2 after Player1 removed"), ThreatComp->GetCurrentAggroTarget(), Cast<AActor>(Player2));
		TestEqual(TEXT("AC-7: Tracked combatant count decreased to 1"), ThreatComp->GetTrackedCombatantCount(), 1);
	}

	// =========================================================================
	// AC-8: GAS Damage & Posture Damage Bridge to Threat
	// =========================================================================
	{
		APAStoneGolemBoss* Boss = NewObject<APAStoneGolemBoss>();
		UPAThreatComponent* ThreatComp = Boss->GetThreatComponent();
		APABaseCharacter* Attacker = NewObject<APABaseCharacter>();

		// Mô phỏng nhận sát thương máu từ delegate GAS
		Boss->HandleDamageReceived(Attacker, 60.0f);
		TestEqual(TEXT("AC-8: Threat increased by 60 from GAS damage"), ThreatComp->GetThreat(Attacker), 60.0f);

		// Mô phỏng nhận sát thương Posture từ delegate GAS (2.5x multiplier)
		Boss->HandlePostureDamageReceived(Attacker, 20.0f);
		// 60 + 20 * 2.5 = 60 + 50 = 110
		TestEqual(TEXT("AC-8: Threat increased by 50 from Posture damage (2.5x)"), ThreatComp->GetThreat(Attacker), 110.0f);
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
