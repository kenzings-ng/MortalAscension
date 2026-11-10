// Copyright Project Ascendant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Progression/PAClassProgressionTypes.h"
#include "ClassPromotionScrollDefinition.generated.h"

/**
 * UClassPromotionScrollDefinition
 *
 * Data Asset định nghĩa Quyển Trục Thăng Chức (Class Promotion Scroll).
 * Khớp 100% với đặc tả tại design/gdd/advanced-classes.md §4.1 và ROADMAP.md:80.
 *
 * Kiểm soát thăng tiến Bậc Chức Nghiệp (T1 -> T2 -> T3 -> T4)
 * và điều kiện kỳ ngộ đa nhánh cho Apex Class (God Slayer).
 */
UCLASS(BlueprintType)
class PROJECTASCENDANT_API UClassPromotionScrollDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UClassPromotionScrollDefinition();

	/** Định danh duy nhất của quyển trục */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion")
	FName ScrollID;

	/** Thẻ chức nghiệp đích mà quyển trục thăng tiến tới */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion")
	FGameplayTag TargetClassTag;

	/** Danh sách các chức nghiệp tiền đề được phép thăng tiến lên TargetClassTag */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion")
	TArray<FGameplayTag> AllowedSourceClassTags;

	/** Bậc tối thiểu của chức nghiệp nguồn (T1, T2, T3) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion")
	uint8 RequiredMinRank = 1;

	/** Cấp độ tối thiểu của chức nghiệp nguồn (mặc định 20) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion")
	int32 RequiredSourceClassLevel = 20;

	/** Cờ nhiệm vụ / kỳ ngộ dã ngoại bắt buộc */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion")
	FGameplayTag RequiredQuestFlag;

	/** Cờ đánh dấu yêu cầu điều kiện đồng thời trên cả 2 slot (cho Apex Class - God Slayer) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion|Apex")
	bool bRequiresDualSlotCondition = false;

	/** Danh sách chức nghiệp nguồn phụ hợp lệ cho slot thứ hai */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion|Apex", meta = (EditCondition = "bRequiresDualSlotCondition"))
	TArray<FGameplayTag> SecondaryAllowedSourceTags;

	/** Bậc tối thiểu yêu cầu cho slot thứ hai */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Promotion|Apex", meta = (EditCondition = "bRequiresDualSlotCondition"))
	uint8 SecondaryRequiredMinRank = 2;

	// -------------------------------------------------------------------------
	// Tiện ích Thẩm định & Kinh tế MVP (advanced-classes.md §5, DECISIONS §6, §12)
	// -------------------------------------------------------------------------

	/**
	 * Kiểm tra điều kiện thăng chức đơn slot.
	 */
	UFUNCTION(BlueprintPure, Category = "Promotion")
	bool CanPromote(
		const FGameplayTag& CurrentClassTag,
		int32 CurrentClassLevel,
		const FGameplayTagContainer& OwnedQuestFlags
	) const;

	/**
	 * Kiểm tra điều kiện thăng chức đa slot (Apex - God Slayer).
	 */
	UFUNCTION(BlueprintPure, Category = "Promotion")
	bool CanPromoteDualSlot(
		const FGameplayTag& PrimaryTag,
		int32 PrimaryLevel,
		const FGameplayTag& SecondaryTag,
		int32 SecondaryLevel,
		const FGameplayTagContainer& OwnedQuestFlags
	) const;

	/**
	 * Số lượng Tàn Trang (item_skill_shard) nhận được khi phân rã (advanced-classes.md §5).
	 * T2: 3 Tàn Trang, T3: 8 Tàn Trang, T4: 25 Tàn Trang.
	 */
	UFUNCTION(BlueprintPure, Category = "Promotion|Economy")
	int32 GetSalvageShardCount() const;

	/**
	 * Giá bán cho NPC Thương nhân lấy vàng trong giai đoạn MVP (DECISIONS.md §6).
	 */
	UFUNCTION(BlueprintPure, Category = "Promotion|Economy")
	int32 GetVendorGoldPrice() const;

	/**
	 * Giá bán lại tại Chợ Đen Cấm Địa.
	 */
	UFUNCTION(BlueprintPure, Category = "Promotion|Economy")
	int32 GetBlackMarketBuybackPrice() const;
};
