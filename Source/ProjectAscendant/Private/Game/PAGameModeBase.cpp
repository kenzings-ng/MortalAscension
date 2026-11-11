// Copyright Project Ascendant. All Rights Reserved.

#include "Game/PAGameModeBase.h"
#include "Character/PABaseCharacter.h"
#include "Controller/PABasePlayerController.h"
#include "Account/PAAccountSubsystem.h"
#include "Backend/PABackendPersistenceSubsystem.h"
#include "Inventory/PAInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "TimerManager.h"

APAGameModeBase::APAGameModeBase()
{
	DefaultPawnClass = APABaseCharacter::StaticClass();
	PlayerControllerClass = APABasePlayerController::StaticClass();
}

void APAGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Trên Dedicated Server hoặc khi truyền tham số -requireauth, bắt buộc kiểm tra token
	if (IsRunningDedicatedServer() || FParse::Param(FCommandLine::Get(), TEXT("requireauth")))
	{
		bRequireAuthToken = true;
	}

	// Bật chu kỳ tự động lưu định kỳ nếu thời gian > 0
	if (AutoSaveIntervalSeconds > 0.0f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			AutoSaveTimerHandle,
			this,
			&APAGameModeBase::TriggerPeriodicAutoSave,
			AutoSaveIntervalSeconds,
			true
		);
		UE_LOG(LogTemp, Log, TEXT("[APAGameModeBase] Periodic Auto-Save active every %.1f seconds."), AutoSaveIntervalSeconds);
	}
}

void APAGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoSaveTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void APAGameModeBase::PreLogin(
	const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	// Trích xuất mã xác thực từ chuỗi tham số kết nối (URL Options: ?token=... hoặc ?AuthToken=...)
	FString Token = UGameplayStatics::ParseOption(Options, TEXT("token"));
	if (Token.IsEmpty())
	{
		Token = UGameplayStatics::ParseOption(Options, TEXT("AuthToken"));
	}

	if (bRequireAuthToken)
	{
		if (Token.IsEmpty())
		{
			ErrorMessage = TEXT("Authentication failed: Missing session token.");
			UE_LOG(LogTemp, Warning, TEXT("[APAGameModeBase::PreLogin] Connection rejected from %s: Missing token."), *Address);
			return;
		}

		// Kiểm tra tính hợp lệ cơ bản của token (định dạng PA-TOKEN- hoặc JWT RFC 7519)
		const bool bIsValidTokenFormat = Token.StartsWith(TEXT("PA-TOKEN-")) || (Token.CountChar(TEXT('.')) == 2);
		if (!bIsValidTokenFormat)
		{
			ErrorMessage = TEXT("Authentication failed: Invalid token format.");
			UE_LOG(LogTemp, Warning, TEXT("[APAGameModeBase::PreLogin] Connection rejected from %s: Malformed token."), *Address);
			return;
		}
	}

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void APAGameModeBase::Logout(AController* Exiting)
{
	// Tự động lưu dữ liệu nhân vật khi người chơi thoát hoặc mất kết nối (Disconnect-Save)
	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		UE_LOG(LogTemp, Log, TEXT("[APAGameModeBase::Logout] Player %s disconnected. Triggering Disconnect-Save..."), *PC->GetName());
		TriggerImmediateCharacterSave(PC);
	}

	Super::Logout(Exiting);
}

void APAGameModeBase::TriggerPeriodicAutoSave()
{
	if (!GetWorld())
	{
		return;
	}

	int32 SavedCount = 0;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			TriggerImmediateCharacterSave(PC);
			SavedCount++;
		}
	}

	if (SavedCount > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[APAGameModeBase] Periodic Auto-Save triggered for %d active player(s)."), SavedCount);
	}
}

void APAGameModeBase::TriggerImmediateCharacterSave(APlayerController* PC)
{
	if (!PC)
	{
		return;
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	UPABackendPersistenceSubsystem* BackendSubsystem = GI->GetSubsystem<UPABackendPersistenceSubsystem>();
	if (!BackendSubsystem)
	{
		return;
	}

	APABaseCharacter* BaseChar = Cast<APABaseCharacter>(PC->GetPawn());
	if (!BaseChar)
	{
		return;
	}

	FPABackendCharacterData CharData;
	CharData.CharacterId = BaseChar->GetName();
	CharData.CharacterLevel = 1;
	CharData.CharacterExp = 0;
	CharData.PrimaryClassTag = TEXT("Class.Line.Guard.Vanguard");

	// Thu thập dữ liệu túi đồ nếu nhân vật sở hữu UPAInventoryComponent
	if (UPAInventoryComponent* InvComp = BaseChar->FindComponentByClass<UPAInventoryComponent>())
	{
		const int32 MaxSlots = InvComp->GetCurrentMaxSlots();
		for (int32 i = 0; i < MaxSlots; ++i)
		{
			if (const FPAInventoryItemEntry* Entry = InvComp->GetItemAtSlot(i))
			{
				FPASavedItemInstance SavedItem;
				SavedItem.ItemInstanceUID = Entry->ItemInstanceUID;
				SavedItem.ItemDefId = Entry->ItemDefinitionId;
				SavedItem.SlotIndex = i;
				SavedItem.StackCount = Entry->Quantity;
				SavedItem.CurrentDurability = Entry->InstanceData.CurrentDurability;
				SavedItem.EnhancementLevel = Entry->InstanceData.EnhancementLevel;
				CharData.Items.Add(SavedItem);
			}
		}
	}

	// Gọi bất đồng bộ tới Backend Game Service thông qua FHttpModule
	BackendSubsystem->SaveCharacter(CharData, FPABackendSaveCharacterDelegate::CreateLambda(
		[CharId = CharData.CharacterId](bool bSuccess, const FString& Message)
		{
			if (bSuccess)
			{
				UE_LOG(LogTemp, Log, TEXT("[APAGameModeBase] Auto-Save completed successfully for %s."), *CharId);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[APAGameModeBase] Auto-Save failed for %s: %s"), *CharId, *Message);
			}
		}));
}
