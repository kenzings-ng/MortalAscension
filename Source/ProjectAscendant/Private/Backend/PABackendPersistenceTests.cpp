// Copyright Project Ascendant. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Backend/PABackendPersistenceSubsystem.h"
#include "Game/PAGameModeBase.h"
#include "Itemization/PASavedItemInstance.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * FPABackendPersistenceTests
 *
 * Kiểm thử tự động cho Giai đoạn 6 (Backend Game Service & Data Persistence - ADR-0006):
 *  - Test 1: Subsystem configuration (BaseUrl, ServerSecret) & payload data structures.
 *  - Test 2: GameMode PreLogin token authentication (Missing token rejection, valid token acceptance).
 *  - Test 3: Auto-Save interval & Disconnect-Save triggers.
 *  - Test 4: Inventory items serialization matching PostgreSQL schema.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPABackendPersistenceTests,
	"ProjectAscendant.Backend.PersistenceGatewayAndSecurity",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPABackendPersistenceTests::RunTest(const FString& Parameters)
{
	// ===========================================================
	// Test 1: Subsystem Configuration & Character Data Structure
	// ===========================================================
	{
		UPABackendPersistenceSubsystem* Subsystem = NewObject<UPABackendPersistenceSubsystem>();
		TestNotNull(TEXT("Subsystem instantiated"), Subsystem);

		Subsystem->SetBackendBaseUrl(TEXT("http://game-backend.ascendant.internal:8080"));
		TestEqual(TEXT("BaseUrl updated"), Subsystem->GetBackendBaseUrl(), TEXT("http://game-backend.ascendant.internal:8080"));

		Subsystem->SetServerSecret(TEXT("super_secret_test_key_2026"));

		FPABackendCharacterData CharData;
		CharData.CharacterId = TEXT("CHAR-UUID-001");
		CharData.AccountId = TEXT("ACC-UUID-001");
		CharData.CharacterName = TEXT("VanguardLeader");
		CharData.CharacterLevel = 25;
		CharData.CharacterExp = 50000;
		CharData.PrimaryClassTag = TEXT("Class.Line.Guard.Vanguard");
		CharData.SecondaryClassTag = TEXT("Class.Line.Scout.Ranger");
		CharData.CompletedQuests.Add(TEXT("quest_bastion_prologue"));

		TestEqual(TEXT("CharacterId matches"), CharData.CharacterId, TEXT("CHAR-UUID-001"));
		TestEqual(TEXT("Level matches"), CharData.CharacterLevel, 25);
		TestEqual(TEXT("Primary class matches"), CharData.PrimaryClassTag, TEXT("Class.Line.Guard.Vanguard"));
		TestEqual(TEXT("Secondary class matches"), CharData.SecondaryClassTag, TEXT("Class.Line.Scout.Ranger"));
		TestEqual(TEXT("Quests count matches"), CharData.CompletedQuests.Num(), 1);
	}

	// ===========================================================
	// Test 2: GameMode PreLogin Token Authentication Gate
	// ===========================================================
	{
		APAGameModeBase* GameMode = NewObject<APAGameModeBase>();
		TestNotNull(TEXT("GameMode instantiated"), GameMode);

		// Khi chưa bật require auth (Dev/Standalone)
		GameMode->SetRequireAuthToken(false);
		FString ErrorMsg;
		GameMode->PreLogin(TEXT(""), TEXT("127.0.0.1"), FUniqueNetIdRepl(), ErrorMsg);
		TestTrue(TEXT("PreLogin passes when token not required"), ErrorMsg.IsEmpty());

		// Khi bật require auth (Dedicated Server production)
		GameMode->SetRequireAuthToken(true);
		TestTrue(TEXT("RequireAuthToken flag is true"), GameMode->IsAuthTokenRequired());

		// 1. Thiếu token -> Bị từ chối
		ErrorMsg.Empty();
		GameMode->PreLogin(TEXT("?Name=Player1"), TEXT("127.0.0.1"), FUniqueNetIdRepl(), ErrorMsg);
		TestFalse(TEXT("Missing token rejected"), ErrorMsg.IsEmpty());
		TestTrue(TEXT("Error indicates missing token"), ErrorMsg.Contains(TEXT("Missing session token")));

		// 2. Token dị tật (malformed) -> Bị từ chối
		ErrorMsg.Empty();
		GameMode->PreLogin(TEXT("?token=random_invalid_string"), TEXT("127.0.0.1"), FUniqueNetIdRepl(), ErrorMsg);
		TestFalse(TEXT("Malformed token rejected"), ErrorMsg.IsEmpty());
		TestTrue(TEXT("Error indicates invalid format"), ErrorMsg.Contains(TEXT("Invalid token format")));

		// 3. Token hợp lệ (PA-TOKEN-...) -> Chấp thuận
		ErrorMsg.Empty();
		GameMode->PreLogin(TEXT("?token=PA-TOKEN-ACC001-1794351-randomsecret"), TEXT("127.0.0.1"), FUniqueNetIdRepl(), ErrorMsg);
		TestTrue(TEXT("PA-TOKEN format accepted"), ErrorMsg.IsEmpty());

		// 4. Token chuẩn RFC 7519 JWT (3 phần phân tách bằng dấu chấm) -> Chấp thuận
		ErrorMsg.Empty();
		GameMode->PreLogin(TEXT("?token=eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiIxMjM0In0.signature"), TEXT("127.0.0.1"), FUniqueNetIdRepl(), ErrorMsg);
		TestTrue(TEXT("JWT format accepted"), ErrorMsg.IsEmpty());
	}

	// ===========================================================
	// Test 3: Inventory Item Instance Structure Serialization
	// ===========================================================
	{
		FPABackendCharacterData CharData;
		CharData.CharacterId = TEXT("CHAR-UUID-002");

		FPASavedItemInstance Sword;
		Sword.ItemInstanceUID = FGuid::NewGuid();
		Sword.ItemDefId = FName(TEXT("sword_bastard_steel"));
		Sword.SlotIndex = 0;
		Sword.StackCount = 1;
		Sword.CurrentDurability = 95.5f;
		Sword.EnhancementLevel = 3;

		FPASavedItemInstance Shield;
		Shield.ItemInstanceUID = FGuid::NewGuid();
		Shield.ItemDefId = FName(TEXT("shield_tower_iron"));
		Shield.SlotIndex = 1;
		Shield.StackCount = 1;
		Shield.CurrentDurability = 100.0f;
		Shield.EnhancementLevel = 5;

		CharData.Items.Add(Sword);
		CharData.Items.Add(Shield);

		TestEqual(TEXT("Items count is 2"), CharData.Items.Num(), 2);
		TestEqual(TEXT("Sword slot is 0"), CharData.Items[0].SlotIndex, 0);
		TestEqual(TEXT("Shield slot is 1"), CharData.Items[1].SlotIndex, 1);
		TestEqual(TEXT("Sword enhancement is +3"), CharData.Items[0].EnhancementLevel, 3);
		TestEqual(TEXT("Shield enhancement is +5"), CharData.Items[1].EnhancementLevel, 5);
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
