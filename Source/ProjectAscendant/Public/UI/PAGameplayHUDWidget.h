// Copyright Project Ascendant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/PACombatTextTypes.h"
#include "UI/PAShopForgeUITypes.h"
#include "PAGameplayHUDWidget.generated.h"

class APABasePlayerController;
class APABaseCharacter;
class UPAPlayerVitalsWidget;
class UPABossHealthWidget;
class UPAMerchantShopWidget;
class UPABlacksmithForgeWidget;
class UPAFloatingCombatTextComponent;
class UPAMerchantComponent;
class UPABlacksmithComponent;
class UPAInventoryComponent;
class UPACurrencyComponent;
class UPAServiceRequestComponent;

/**
 * UPAGameplayHUDWidget
 *
 * Widget giao diện trung tâm trong game (In-Game HUD) của Project Ascendant.
 * Tích hợp toàn diện các cụm giao diện theo GDD combat-hud.md:
 * - Cụm Sinh mệnh người chơi (Player Cluster - UPAPlayerVitalsWidget): Máu, Ghost Bar, Thể lực, Mana, Kiệt sức.
 * - Cụm Trùm thế giới (Boss Cluster - UPABossHealthWidget): Thanh máu trùm, thanh thế đứng, tâm kết liễu, gãy bộ phận.
 * - Số sát thương nổi (Floating Combat Text - UPAFloatingCombatTextComponent).
 * - Cửa sổ tương tác thương nhân (UPAMerchantShopWidget) và lò rèn (UPABlacksmithForgeWidget).
 * - Nhãn gợi ý phím tương tác ngữ cảnh [F] (Interaction Prompt) trong phạm vi 300cm.
 *
 * Tham chiếu GDD:
 * - design/gdd/combat-hud.md
 * - design/gdd/merchant-economy.md
 * - design/gdd/blacksmithing-system.md
 * - DECISIONS.md §13: Bán kính tương tác với merchant/forge: 300cm.
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTASCENDANT_API UPAGameplayHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	// ===========================================================
	// Initialization & Binding
	// ===========================================================

	/**
	 * Khởi tạo HUD với PlayerController và Character sở hữu.
	 * Tự động liên kết PlayerVitalsWidget vào GAS AttributeSet.
	 */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD")
	void InitializeHUD(APABasePlayerController* InPC, APABaseCharacter* InCharacter);

	// ===========================================================
	// Child Widget Accessors
	// ===========================================================

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|HUD")
	UPAPlayerVitalsWidget* GetVitalsWidget() const { return VitalsWidget; }

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|HUD")
	UPABossHealthWidget* GetBossHealthWidget() const { return BossHealthWidget; }

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|HUD")
	UPAMerchantShopWidget* GetShopWidget() const { return ShopWidget; }

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|HUD")
	UPABlacksmithForgeWidget* GetForgeWidget() const { return ForgeWidget; }

	// ===========================================================
	// Boss Encounter HUD Control
	// ===========================================================

	/** Hiển thị thanh máu và thế đứng trùm */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD|Boss")
	void ShowBossHUD(AActor* BossActor, const FString& BossName, int32 BossLevel, const TArray<FName>& PartIds);

	/** Ẩn thanh máu trùm khi kết thúc giao tranh */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD|Boss")
	void HideBossHUD();

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|HUD|Boss")
	bool IsBossHUDVisible() const { return bIsBossHUDActive; }

	// ===========================================================
	// Floating Combat Text
	// ===========================================================

	/** Spawn số sát thương nổi qua component */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD|CombatText")
	void SpawnCombatText(const FVector& WorldLocation, float Amount, EPACombatTextType Type);

	/** Spawn chữ "PERFECT!" khi né hoàn hảo */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD|CombatText")
	void SpawnPerfectDodgeCallout(const FVector& CharacterLocation);

	// ===========================================================
	// Interaction Prompt [F]
	// ===========================================================

	/** Cập nhật nội dung hiển thị nút bấm tương tác (VD: "[F] Giao Dịch với Thương Nhân") */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD|Interaction")
	void SetInteractionPrompt(const FString& PromptText, bool bVisible);

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|HUD|Interaction")
	const FString& GetInteractionPromptText() const { return CurrentPromptText; }

	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|HUD|Interaction")
	bool IsInteractionPromptVisible() const { return bIsPromptVisible; }

	// ===========================================================
	// Interactive Modal Windows (Shop & Forge)
	// ===========================================================

	/** Mở giao diện cửa hàng thương nhân */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD|Shop")
	void OpenShop(UPAMerchantComponent* MerchantComp, UPAInventoryComponent* PlayerInv,
		UPACurrencyComponent* PlayerWallet, int32 PlayerKarma, UPAServiceRequestComponent* RequestRouter = nullptr);

	/** Mở giao diện bệ đe rèn */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD|Forge")
	void OpenForge(UPABlacksmithComponent* ForgeComp, UPAInventoryComponent* PlayerInv,
		UPACurrencyComponent* PlayerWallet, UPAServiceRequestComponent* RequestRouter = nullptr);

	/** Đóng mọi cửa sổ tương tác đang mở */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|HUD")
	void CloseInteractiveWindows();

	/** Kiểm tra có cửa sổ tương tác nào đang mở hay không */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|HUD")
	bool IsAnyInteractiveWindowOpen() const;

	// ===========================================================
	// Sub-Widget Classes (Configurable via Blueprint subclass)
	// ===========================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectAscendant|HUD|Classes")
	TSubclassOf<UPAPlayerVitalsWidget> VitalsWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectAscendant|HUD|Classes")
	TSubclassOf<UPABossHealthWidget> BossHealthWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectAscendant|HUD|Classes")
	TSubclassOf<UPAMerchantShopWidget> ShopWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectAscendant|HUD|Classes")
	TSubclassOf<UPABlacksmithForgeWidget> ForgeWidgetClass;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	TObjectPtr<UPAPlayerVitalsWidget> VitalsWidget;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	TObjectPtr<UPABossHealthWidget> BossHealthWidget;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	TObjectPtr<UPAMerchantShopWidget> ShopWidget;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	TObjectPtr<UPABlacksmithForgeWidget> ForgeWidget;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	TWeakObjectPtr<APABasePlayerController> OwningPC;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	TWeakObjectPtr<APABaseCharacter> OwningCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	FString CurrentPromptText;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	bool bIsPromptVisible = false;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	bool bIsBossHUDActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	bool bIsShopOpen = false;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectAscendant|HUD")
	bool bIsForgeOpen = false;

private:
	void EnsureChildWidgetsCreated();
};
