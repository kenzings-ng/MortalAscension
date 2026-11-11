// Copyright Project Ascendant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Itemization/PASavedItemInstance.h"
#include "PABackendPersistenceSubsystem.generated.h"

/**
 * FPABackendCharacterData
 *
 * Payload đại diện cho dữ liệu nhân vật được tải hoặc lưu qua Backend REST API.
 */
USTRUCT(BlueprintType)
struct PROJECTASCENDANT_API FPABackendCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	FString CharacterId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	FString AccountId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	int32 CharacterLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	int64 CharacterExp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	FString PrimaryClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	FString SecondaryClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	TArray<FString> CompletedQuests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Backend")
	TArray<FPASavedItemInstance> Items;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPABackendTokenValidationDelegate, bool, bIsValid, const FString&, AccountId);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPABackendSaveCharacterDelegate, bool, bSuccess, const FString&, Message);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPABackendLoadCharacterDelegate, bool, bSuccess, const FPABackendCharacterData&, LoadedData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPABackendPromotionDelegate, bool, bSuccess, const FString&, Message);

/**
 * UPABackendPersistenceSubsystem
 *
 * GameInstance Subsystem cung cấp cầu nối HTTP giao tiếp giữa Unreal Engine Dedicated Server
 * và Python FastAPI Backend Game Service (ADR-0006).
 *
 * Chỉ Dedicated Server được phép gọi các API ghi dữ liệu (FHttpModule).
 * Client không bao giờ ghi trực tiếp vào cơ sở dữ liệu.
 */
UCLASS(Config = Game)
class PROJECTASCENDANT_API UPABackendPersistenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Cấu hình Base URL của Backend Service (mặc định http://localhost:8080) */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Backend")
	void SetBackendBaseUrl(const FString& InUrl) { BackendBaseUrl = InUrl; }

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Backend")
	FString GetBackendBaseUrl() const { return BackendBaseUrl; }

	/** Cấu hình Secret xác thực giữa Dedicated Server và Backend Service */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Backend")
	void SetServerSecret(const FString& InSecret) { ServerSecret = InSecret; }

	/**
	 * Thẩm tra Session Token của Client khi kết nối vào Dedicated Server (PreLogin).
	 */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Backend")
	void ValidateClientToken(const FString& Token, const FString& ExpectedAccountId, FPABackendTokenValidationDelegate Callback);

	/**
	 * Tải dữ liệu nhân vật và túi đồ từ PostgreSQL thông qua Backend Service.
	 */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Backend")
	void LoadCharacter(const FString& CharacterId, FPABackendLoadCharacterDelegate Callback);

	/**
	 * Lưu trạng thái nhân vật và vật phẩm định kỳ hoặc khi người chơi ngắt kết nối (Auto-Save / Disconnect-Save).
	 */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Backend")
	void SaveCharacter(const FPABackendCharacterData& CharacterData, FPABackendSaveCharacterDelegate Callback);

	/**
	 * Hoán đổi nguyên tử hai ô đồ thông qua Backend Service (chống dupe).
	 */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Backend")
	void SwapInventorySlots(const FString& OwnerId, const FString& SlotType, int32 SlotA, int32 SlotB, FPABackendSaveCharacterDelegate Callback);

	/**
	 * Yêu cầu thăng chức nhân vật bằng quyển trục với khóa dòng nguyên tử (SELECT ... FOR UPDATE).
	 */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Backend")
	void PromoteClassWithScroll(const FString& CharacterId, const FGuid& ScrollInstanceId, const FString& TargetClassTag, bool bIsSecondary, FPABackendPromotionDelegate Callback);

private:
	UPROPERTY(Config)
	FString BackendBaseUrl = TEXT("http://localhost:8080");

	UPROPERTY(Config)
	FString ServerSecret = TEXT("ascendant_dedicated_server_secret_2026");
};
