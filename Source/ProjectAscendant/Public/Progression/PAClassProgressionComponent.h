// Copyright Project Ascendant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Progression/PAClassProgressionTypes.h"
#include "PAClassProgressionComponent.generated.h"

class UClassPromotionScrollDefinition;

/**
 * UPAClassProgressionComponent
 *
 * Component quản lý tiến trình Song Chức Nghiệp (Dual-Class System) và Thăng Chức Bằng Quyển Trục.
 * Tuân thủ tuyệt đối:
 * - ADR-0001: 100% Server-Authoritative.
 * - DECISIONS.md §1, §2, §3, §4, §7, §8, §12.
 * - design/gdd/advanced-classes.md.
 * - ROADMAP.md:75-87 (Giai đoạn 2B).
 *
 * Tính năng chính:
 * 1. Slot Chính (Primary) & Phụ (Secondary) replicated qua network.
 * 2. Lưu trữ vĩnh viễn tiến trình từng class đã chơi (Class Level 1-20, Class EXP).
 * 3. Phân phối EXP kép: 100% vào Character Level, 70% vào Class Chính, 30% vào Class Phụ.
 * 4. Quy tắc A1: Hoán đổi Chính <-> Phụ tại Tòa Thành (Citadel), tự động clamp bậc Phụ <= bậc Chính.
 * 5. Quy tắc B1: Slot F của Class Phụ chỉ dùng kỹ năng Không Phụ Thuộc Vũ Khí nếu khác họ vũ khí.
 * 6. Server RPC thăng chức bằng Quyển trục Data Asset (kiểm tra điều kiện đơn slot & song slot cho Apex).
 * 7. Kiểm soát trần cộng dồn Nội tại (Passive Matrix Caps).
 */
UCLASS(ClassGroup = (ProjectAscendant), meta = (BlueprintSpawnableComponent))
class PROJECTASCENDANT_API UPAClassProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPAClassProgressionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// -------------------------------------------------------------------------
	// Public API — Queries & Getters
	// -------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	FGameplayTag GetPrimaryClassTag() const { return PrimaryClassTag; }

	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	FGameplayTag GetSecondaryClassTag() const { return SecondaryClassTag; }

	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	EPAClassRank GetPrimaryClassRank() const;

	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	EPAClassRank GetSecondaryClassRank() const;

	/**
	 * Bậc thực tế của Class Phụ sau khi clamp theo Bậc Class Chính (Quy tắc A1).
	 * Rank_Sub <= Rank_Primary.
	 */
	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	EPAClassRank GetEffectiveSecondaryRank() const;

	/**
	 * Kiểm tra xem một class đã được mở khóa chưa.
	 */
	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	bool IsClassUnlocked(const FGameplayTag& InClassTag) const;

	/**
	 * Lấy cấp độ Class Level (1-20) của một chức nghiệp.
	 */
	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	int32 GetClassLevel(const FGameplayTag& InClassTag) const;

	/**
	 * Lấy lượng Class XP hiện tại của một chức nghiệp.
	 */
	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	int32 GetClassXP(const FGameplayTag& InClassTag) const;

	/**
	 * Kiểm tra xem nhân vật hiện có đang ở trong Tòa Thành / Sanctuary an toàn hay không.
	 */
	UFUNCTION(BlueprintPure, Category = "Progression|Citadel")
	bool IsInCitadel() const;

	/**
	 * Thiết lập cờ trạng thái Tòa Thành (dùng cho server & testing).
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Citadel")
	void SetInCitadel(bool bInCitadel);

	// -------------------------------------------------------------------------
	// Public API — Server-Authoritative Progression
	// -------------------------------------------------------------------------

	/**
	 * Khởi tạo chức nghiệp khởi đầu (thường là T1 như Vanguard, Ranger, Arcanist, Acolyte).
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Class")
	bool InitializeFoundationalClass(const FGameplayTag& InClassTag);

	/**
	 * Gán Class Phụ (Secondary Sub-Class) cho nhân vật (DECISIONS.md §3).
	 * Khi chọn class mới, tiến trình class cũ được bảo toàn vĩnh viễn trong ProgressHistory.
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Class")
	bool SetSecondaryClass(const FGameplayTag& InClassTag);

	/**
	 * Gán Class Chính (Primary Class) cho nhân vật.
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Class")
	bool SetPrimaryClass(const FGameplayTag& InClassTag);

	/**
	 * Phân phối Class EXP từ chiến đấu:
	 * - 70% nạp vào Class Chính.
	 * - 30% nạp vào Class Phụ (nếu có).
	 * @param TotalCombatClassXP Tổng lượng Class EXP nhận được.
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Class")
	void GrantCombatClassXP(int32 TotalCombatClassXP);

	/**
	 * Quy tắc A1: Hoán đổi vị trí Class Chính <-> Class Phụ tại Tòa Thành.
	 * @param OutError Mã lỗi nếu thất bại.
	 * @return true nếu hoán đổi thành công.
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Class")
	bool SwapPrimarySecondaryClasses(EPAClassPromotionError& OutError);

	/**
	 * Quy tắc A1 Validation: Kiểm tra xem có đủ điều kiện hoán đổi Chính <-> Phụ hay không.
	 */
	UFUNCTION(BlueprintPure, Category = "Progression|Class")
	bool CanSwapPrimarySecondary() const;

	bool CanSwapPrimarySecondary(FString& OutFailReason) const;

	/**
	 * Quy tắc B1: Thẩm định xem một kỹ năng chủ động của Class Phụ có được phép kích hoạt ở Slot F hay không.
	 * Nếu khác họ vũ khí, chỉ cho phép kỹ năng Weapon-Agnostic.
	 */
	UFUNCTION(BlueprintPure, Category = "Progression|Combat")
	bool IsSecondaryActiveSkillAllowed(FName AbilityWeaponFamily, bool bIsWeaponAgnostic) const;

	/**
	 * Thăng chức chức nghiệp bằng Quyển trục (Server-Authoritative logic).
	 * Kiểm tra điều kiện cấp độ (Class Level 20), class nguồn, cờ nhiệm vụ, và tiêu thụ quyển trục từ túi đồ (ADR-0003, ROADMAP.md:83).
	 * @param ScrollDef Data Asset quyển trục thăng chức.
	 * @param TargetSlot Slot muốn thăng chức (Primary hoặc Secondary).
	 * @param CharacterFlags Tập cờ nhiệm vụ của nhân vật để kiểm tra kỳ ngộ.
	 * @return EPAClassPromotionError::None nếu thành công.
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Promotion")
	EPAClassPromotionError ExecutePromotion(
		const UClassPromotionScrollDefinition* ScrollDef,
		EPAClassSlot TargetSlot,
		const FGameplayTagContainer& CharacterFlags
	);

	/**
	 * Wrapper C++ / Blueprint gọi ExecutePromotion.
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Promotion")
	EPAClassPromotionError PromoteClass(
		const UClassPromotionScrollDefinition* ScrollDef,
		EPAClassSlot TargetSlot,
		const FGameplayTagContainer& CharacterFlags
	)
	{
		return ExecutePromotion(ScrollDef, TargetSlot, CharacterFlags);
	}

	/**
	 * Server RPC cho remote client yêu cầu thăng chức chức nghiệp (ADR-0001, ROADMAP.md:83).
	 * Server tự động xác thực bằng OwnedQuestFlags và tiêu thụ quyển trục qua InventoryComponent.
	 */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Progression|Promotion")
	void ServerPromoteClass(
		const UClassPromotionScrollDefinition* ScrollDef,
		EPAClassSlot TargetSlot
	);

	/**
	 * Đăng ký cờ nhiệm vụ cho nhân vật (dùng cho kỳ ngộ / Apex requirements).
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression|Quests")
	void AddQuestFlag(const FGameplayTag& FlagTag);

	UFUNCTION(BlueprintPure, Category = "Progression|Quests")
	const FGameplayTagContainer& GetQuestFlags() const { return OwnedQuestFlags; }

	// -------------------------------------------------------------------------
	// Action Deck Slot Helpers (design/gdd/advanced-classes.md §2)
	// -------------------------------------------------------------------------

	/** Slot Q, E, R thuộc Class Chính */
	UFUNCTION(BlueprintPure, Category = "Progression|ActionDeck")
	bool IsPrimaryActiveSlot(FName SlotName) const;

	/** Slot F thuộc Class Phụ */
	UFUNCTION(BlueprintPure, Category = "Progression|ActionDeck")
	bool IsSecondaryActiveSlot(FName SlotName) const;

	/** Slot P1, P2 thuộc Class Chính, P3 thuộc Class Phụ */
	UFUNCTION(BlueprintPure, Category = "Progression|ActionDeck")
	EPAClassSlot GetPassiveSlotOwner(FName SlotName) const;

	// -------------------------------------------------------------------------
	// Delegates
	// -------------------------------------------------------------------------

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnClassPromoted, EPAClassSlot /* Slot */, const FGameplayTag& /* NewClassTag */);
	FOnClassPromoted OnClassPromoted;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnClassPromotionFailed, EPAClassSlot /* Slot */, EPAClassPromotionError /* Error */);
	FOnClassPromotionFailed OnClassPromotionFailed;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnClassesSwapped, const FGameplayTag& /* NewPrimary */, const FGameplayTag& /* NewSecondary */);
	FOnClassesSwapped OnClassesSwapped;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnClassXPGained, const FGameplayTag& /* ClassTag */, int32 /* XPAmount */, int32 /* NewClassLevel */);
	FOnClassXPGained OnClassXPGained;

protected:
	// -------------------------------------------------------------------------
	// Replicated State
	// -------------------------------------------------------------------------

	UPROPERTY(ReplicatedUsing = OnRep_PrimaryClassTag)
	FGameplayTag PrimaryClassTag;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryClassTag)
	FGameplayTag SecondaryClassTag;

	UPROPERTY(ReplicatedUsing = OnRep_ProgressHistory)
	TArray<FPAClassProgressRecord> ProgressHistory;

	UPROPERTY(Replicated)
	bool bIsInCitadel = false;

	UPROPERTY(Replicated)
	FGameplayTagContainer OwnedQuestFlags;

	// -------------------------------------------------------------------------
	// Rep Notifies
	// -------------------------------------------------------------------------

	UFUNCTION()
	void OnRep_PrimaryClassTag();

	UFUNCTION()
	void OnRep_SecondaryClassTag();

	UFUNCTION()
	void OnRep_ProgressHistory();

	// -------------------------------------------------------------------------
	// Internal Helpers
	// -------------------------------------------------------------------------

	FPAClassProgressRecord* FindProgressRecord(const FGameplayTag& InTag);
	const FPAClassProgressRecord* FindProgressRecord(const FGameplayTag& InTag) const;
	void AddOrUpdateClassProgress(const FGameplayTag& InTag, int32 LevelDelta, int32 XPDelta);
};
