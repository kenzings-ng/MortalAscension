// Copyright Project Ascendant. All Rights Reserved.

#include "Progression/ClassPromotionScrollDefinition.h"

UClassPromotionScrollDefinition::UClassPromotionScrollDefinition()
{
	ScrollID = NAME_None;
	TargetClassTag = FGameplayTag::EmptyTag;
	RequiredMinRank = 1;
	RequiredSourceClassLevel = 20;
	RequiredQuestFlag = FGameplayTag::EmptyTag;
	bRequiresDualSlotCondition = false;
	SecondaryRequiredMinRank = 2;
}

bool UClassPromotionScrollDefinition::CanPromote(
	const FGameplayTag& CurrentClassTag,
	int32 CurrentClassLevel,
	const FGameplayTagContainer& OwnedQuestFlags) const
{
	if (!TargetClassTag.IsValid())
	{
		return false;
	}

	// 1. Kiểm tra cấp độ nguồn (phải đạt RequiredSourceClassLevel, mặc định 20)
	if (CurrentClassLevel < RequiredSourceClassLevel)
	{
		return false;
	}

	// 2. Kiểm tra Bậc tối thiểu
	const EPAClassRank CurrentRank = FPAClassProgressionHelper::GetClassRank(CurrentClassTag);
	if (static_cast<uint8>(CurrentRank) < RequiredMinRank)
	{
		return false;
	}

	// 3. Kiểm tra Class nguồn có nằm trong danh sách cho phép không
	if (AllowedSourceClassTags.Num() > 0)
	{
		if (!AllowedSourceClassTags.Contains(CurrentClassTag))
		{
			return false;
		}
	}

	// 4. Kiểm tra Cờ Nhiệm Vụ (nếu có yêu cầu)
	if (RequiredQuestFlag.IsValid())
	{
		if (!OwnedQuestFlags.HasTag(RequiredQuestFlag))
		{
			return false;
		}
	}

	return true;
}

bool UClassPromotionScrollDefinition::CanPromoteDualSlot(
	const FGameplayTag& PrimaryTag,
	int32 PrimaryLevel,
	const FGameplayTag& SecondaryTag,
	int32 SecondaryLevel,
	const FGameplayTagContainer& OwnedQuestFlags) const
{
	// Thẩm định slot chính trước
	if (!CanPromote(PrimaryTag, PrimaryLevel, OwnedQuestFlags))
	{
		return false;
	}

	// Nếu yêu cầu điều kiện hai slot (Apex - God Slayer)
	if (bRequiresDualSlotCondition)
	{
		if (!SecondaryTag.IsValid())
		{
			return false;
		}

		const EPAClassRank SecRank = FPAClassProgressionHelper::GetClassRank(SecondaryTag);
		if (static_cast<uint8>(SecRank) < SecondaryRequiredMinRank)
		{
			return false;
		}

		if (SecondaryAllowedSourceTags.Num() > 0)
		{
			if (!SecondaryAllowedSourceTags.Contains(SecondaryTag))
			{
				return false;
			}
		}
	}

	return true;
}

int32 UClassPromotionScrollDefinition::GetSalvageShardCount() const
{
	const EPAClassRank Rank = FPAClassProgressionHelper::GetClassRank(TargetClassTag);
	switch (Rank)
	{
	case EPAClassRank::Intermediate: // T2 Rare
		return 3;
	case EPAClassRank::Master:       // T3 Epic
		return 8;
	case EPAClassRank::Apex:         // T4 Mythic
		return 25;
	default:
		return 1;
	}
}

int32 UClassPromotionScrollDefinition::GetVendorGoldPrice() const
{
	const EPAClassRank Rank = FPAClassProgressionHelper::GetClassRank(TargetClassTag);
	switch (Rank)
	{
	case EPAClassRank::Intermediate: // T2
		return 500;
	case EPAClassRank::Master:       // T3
		return 2500;
	case EPAClassRank::Apex:         // T4
		return 10000;
	default:
		return 100;
	}
}

int32 UClassPromotionScrollDefinition::GetBlackMarketBuybackPrice() const
{
	return FMath::CeilToInt(static_cast<float>(GetVendorGoldPrice()) * 1.5f);
}
