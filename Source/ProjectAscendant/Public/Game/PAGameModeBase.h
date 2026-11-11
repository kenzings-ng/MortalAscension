// Copyright Project Ascendant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PAGameModeBase.generated.h"

class APlayerController;
class UPABackendPersistenceSubsystem;

/**
 * APAGameModeBase
 *
 * GameMode cơ sở cho Project Ascendant (100% Dedicated Server Authority - ADR-0001, ADR-0006).
 * Quản lý xác thực kết nối qua PreLogin, tự động lưu khi ngắt kết nối (Logout),
 * và chu kỳ tự động lưu định kỳ (Auto-Save).
 */
UCLASS()
class PROJECTASCENDANT_API APAGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APAGameModeBase();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Thẩm tra Client Token trước khi cho phép kết nối vào Dedicated Server (Story auth-004, ADR-0006).
	 */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/**
	 * Tự động kích hoạt Disconnect-Save khi người chơi thoát game hoặc mất kết nối mạng.
	 */
	virtual void Logout(AController* Exiting) override;

	/**
	 * Kích hoạt tự động lưu dữ liệu cho toàn bộ người chơi đang kết nối.
	 */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Persistence")
	void TriggerPeriodicAutoSave();

	/**
	 * Kích hoạt lưu dữ liệu tức thì cho một người chơi cụ thể (khi bước vào Citadel Safe Zone).
	 */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Persistence")
	void TriggerImmediateCharacterSave(APlayerController* PC);

	/** Thiết lập yêu cầu bắt buộc phải có Token hợp lệ */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Security")
	void SetRequireAuthToken(bool bRequire) { bRequireAuthToken = bRequire; }

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Security")
	bool IsAuthTokenRequired() const { return bRequireAuthToken; }

	/**
	 * Kiểm tra token xác thực trong chuỗi tham số kết nối, không cần World.
	 * Trả về false và ghi OutErrorMessage khi bị từ chối. Dùng bởi PreLogin và bởi test.
	 */
	static bool CheckAuthToken(const FString& Options, bool bRequireToken, FString& OutErrorMessage);

protected:
	/** Có bắt buộc kiểm tra Token hợp lệ ở PreLogin hay không (bật mặc định trên Dedicated Server phát hành) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Security")
	bool bRequireAuthToken = false;

	/** Khoảng thời gian giữa các lần tự động lưu (giây), mặc định 300s = 5 phút */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Persistence")
	float AutoSaveIntervalSeconds = 300.0f;

	FTimerHandle AutoSaveTimerHandle;
};
