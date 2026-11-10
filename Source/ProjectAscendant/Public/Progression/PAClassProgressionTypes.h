// Copyright Project Ascendant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PAClassProgressionTypes.generated.h"

/**
 * EPAClassRank
 * Bậc Chức Nghiệp (Class Rank) theo DECISIONS.md §1:
 * T1: Sơ cấp (Novice / Foundational)
 * T2: Trung cấp (Intermediate / Advanced)
 * T3: Cao cấp (Master / Specialist)
 * T4: Ẩn / Tối thượng (Apex / Mythic)
 */
UENUM(BlueprintType)
enum class EPAClassRank : uint8
{
	None = 0,
	Novice = 1,       // T1
	Intermediate = 2, // T2
	Master = 3,       // T3
	Apex = 4          // T4
};

/**
 * EPAClassLine
 * 4 Nhánh chính + 1 Nhánh Apex (DECISIONS.md §2).
 */
UENUM(BlueprintType)
enum class EPAClassLine : uint8
{
	None = 0,
	Guard = 1, // Nhánh Hộ Vệ (6 Class)
	Scout = 2, // Nhánh Du Hiệp (3 Class)
	Caster = 3, // Nhánh Pháp Sư (3 Class)
	Faith = 4, // Nhánh Tín Đồ (3 Class)
	Apex = 5   // Nhánh Ẩn Tối Thượng (1 Class Đa Nhánh)
};

/**
 * EPAClassSlot
 * Slot chức nghiệp cho cơ chế Song Chức Nghiệp (Dual-Class System, DECISIONS.md §3).
 */
UENUM(BlueprintType)
enum class EPAClassSlot : uint8
{
	Primary = 0,   // Class Chính
	Secondary = 1  // Class Phụ
};

/**
 * EPAClassPromotionError
 * Mã lỗi khi thực hiện thăng chức hoặc hoán đổi chức nghiệp.
 */
UENUM(BlueprintType)
enum class EPAClassPromotionError : uint8
{
	None = 0,
	NotAuthoritative,
	InvalidScroll,
	RequirementsNotMet,
	MissingQuestFlag,
	ApexDualSlotPrerequisiteNotMet,
	AlreadyAtOrAboveRank,
	NotInCitadel,
	InvalidClassSlot,
	InvalidTargetClass,
	MissingScrollItem
};

/**
 * FPAClassProgressRecord
 * Bản ghi tiến trình độc lập của một chức nghiệp (Class Level 1-20, Class EXP).
 * Lưu vĩnh viễn không bị mất khi đổi sang class khác (DECISIONS.md §3, §4).
 */
USTRUCT(BlueprintType)
struct PROJECTASCENDANT_API FPAClassProgressRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FGameplayTag ClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 ClassLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 ClassXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	bool bUnlocked = false;

	static constexpr int32 MaxClassLevel = 20;
	static constexpr double BaseClassXP = 150.0;
	static constexpr double ClassExponent = 1.5;

	/**
	 * Lượng EXP cần thiết để lên cấp Class tiếp theo (Level 1 -> 20).
	 */
	static int32 GetXPRequiredForClassLevel(int32 CurrentLv)
	{
		if (CurrentLv <= 0 || CurrentLv >= MaxClassLevel)
		{
			return 0;
		}
		const double Raw = BaseClassXP * FMath::Pow(static_cast<double>(CurrentLv), ClassExponent);
		return FMath::CeilToInt(Raw);
	}
};

/**
 * FPAClassMatrixCaps
 * Ngưỡng trần tối đa (Hard Caps) để kiểm soát cân bằng khi kết hợp Nội tại
 * giữa Class Chính và Class Phụ (DECISIONS.md §3, §12).
 */
struct PROJECTASCENDANT_API FPAClassMatrixCaps
{
	static constexpr float MaxDamageReductionCap = 0.60f;     // Giảm sát thương tối đa 60%
	static constexpr float MaxCriticalChanceCap = 0.75f;      // Tỷ lệ bạo kích tối đa 75%
	static constexpr float MaxAttackSpeedBonusCap = 0.50f;    // Tốc độ đánh cộng thêm tối đa 50%
	static constexpr float MaxCooldownReductionCap = 0.40f;   // Giảm hồi chiêu tối đa 40%

	static float ClampDamageReduction(float Value)
	{
		return FMath::Clamp(Value, 0.0f, MaxDamageReductionCap);
	}

	static float ClampCriticalChance(float Value)
	{
		return FMath::Clamp(Value, 0.0f, MaxCriticalChanceCap);
	}

	static float ClampAttackSpeedBonus(float Value)
	{
		return FMath::Clamp(Value, 0.0f, MaxAttackSpeedBonusCap);
	}

	static float ClampCooldownReduction(float Value)
	{
		return FMath::Clamp(Value, 0.0f, MaxCooldownReductionCap);
	}
};

/**
 * FPAClassProgressionHelper
 * Tiện ích truy vấn metadata cho 16 Class từ DECISIONS.md §2, §7, §8.
 */
struct PROJECTASCENDANT_API FPAClassProgressionHelper
{
	static EPAClassRank GetClassRank(const FGameplayTag& Tag)
	{
		const FName TagName = Tag.GetTagName();
		const FString Str = TagName.ToString();

		// T4: Apex
		if (Str.Contains(TEXT("GodSlayer")))
		{
			return EPAClassRank::Apex;
		}

		// T3: DragonKnight, VoidBlade, PhantomStalker, Chronomancer, Seraph
		if (Str.Contains(TEXT("DragonKnight")) || Str.Contains(TEXT("VoidBlade")) ||
			Str.Contains(TEXT("PhantomStalker")) || Str.Contains(TEXT("Chronomancer")) ||
			Str.Contains(TEXT("Seraph")))
		{
			return EPAClassRank::Master;
		}

		// T2: Templar, Berserker, Swordmaster, Shadowblade, Elementalist, Inquisitor
		if (Str.Contains(TEXT("Templar")) || Str.Contains(TEXT("Berserker")) ||
			Str.Contains(TEXT("Swordmaster")) || Str.Contains(TEXT("Shadowblade")) ||
			Str.Contains(TEXT("Elementalist")) || Str.Contains(TEXT("Inquisitor")))
		{
			return EPAClassRank::Intermediate;
		}

		// T1: Vanguard, Ranger, Arcanist, Acolyte
		if (Str.Contains(TEXT("Vanguard")) || Str.Contains(TEXT("Ranger")) ||
			Str.Contains(TEXT("Arcanist")) || Str.Contains(TEXT("Acolyte")))
		{
			return EPAClassRank::Novice;
		}

		return EPAClassRank::None;
	}

	static EPAClassLine GetClassLine(const FGameplayTag& Tag)
	{
		const FString Str = Tag.GetTagName().ToString();
		if (Str.Contains(TEXT("Class.Line.Guard"))) return EPAClassLine::Guard;
		if (Str.Contains(TEXT("Class.Line.Scout"))) return EPAClassLine::Scout;
		if (Str.Contains(TEXT("Class.Line.Caster"))) return EPAClassLine::Caster;
		if (Str.Contains(TEXT("Class.Line.Faith"))) return EPAClassLine::Faith;
		if (Str.Contains(TEXT("Class.Line.Apex"))) return EPAClassLine::Apex;
		return EPAClassLine::None;
	}

	static FName GetClassDefaultWeaponFamily(const FGameplayTag& Tag)
	{
		const FString Str = Tag.GetTagName().ToString();
		if (Str.Contains(TEXT("Vanguard")) || Str.Contains(TEXT("Swordmaster")) || Str.Contains(TEXT("VoidBlade")))
		{
			return FName(TEXT("Weapon.1H.Blade"));
		}
		if (Str.Contains(TEXT("Templar")) || Str.Contains(TEXT("Acolyte")) || Str.Contains(TEXT("Inquisitor")) || Str.Contains(TEXT("Seraph")))
		{
			return FName(TEXT("Weapon.1H.Mace"));
		}
		if (Str.Contains(TEXT("Berserker")))
		{
			return FName(TEXT("Weapon.2H.Heavy"));
		}
		if (Str.Contains(TEXT("DragonKnight")))
		{
			return FName(TEXT("Weapon.2H.Polearm"));
		}
		if (Str.Contains(TEXT("Ranger")))
		{
			return FName(TEXT("Weapon.2H.Bow"));
		}
		if (Str.Contains(TEXT("Shadowblade")) || Str.Contains(TEXT("PhantomStalker")))
		{
			return FName(TEXT("Weapon.Dual.Daggers"));
		}
		if (Str.Contains(TEXT("Arcanist")) || Str.Contains(TEXT("Elementalist")))
		{
			return FName(TEXT("Weapon.2H.Staff"));
		}
		if (Str.Contains(TEXT("Chronomancer")))
		{
			return FName(TEXT("Weapon.Relic"));
		}
		if (Str.Contains(TEXT("GodSlayer")))
		{
			return FName(TEXT("Weapon.1H.Blade"));
		}
		return NAME_None;
	}

	/**
	 * Danh sách họ vũ khí được chức nghiệp hỗ trợ (DECISIONS.md §7).
	 * Riêng God Slayer hỗ trợ cả Weapon.1H.Blade và Weapon.2H.Polearm.
	 * Acolyte & Seraph hỗ trợ Weapon.1H.Mace và Relic.
	 */
	static TArray<FName> GetClassSupportedWeaponFamilies(const FGameplayTag& Tag)
	{
		const FString Str = Tag.GetTagName().ToString();
		if (Str.Contains(TEXT("GodSlayer")))
		{
			return { FName(TEXT("Weapon.1H.Blade")), FName(TEXT("Weapon.2H.Polearm")) };
		}
		if (Str.Contains(TEXT("Acolyte")) || Str.Contains(TEXT("Seraph")))
		{
			return { FName(TEXT("Weapon.1H.Mace")), FName(TEXT("Weapon.Relic")) };
		}
		const FName DefaultFamily = GetClassDefaultWeaponFamily(Tag);
		if (!DefaultFamily.IsNone())
		{
			return { DefaultFamily };
		}
		return {};
	}

	static bool AreWeaponFamiliesCompatible(const FGameplayTag& ClassTagA, const FGameplayTag& ClassTagB)
	{
		const TArray<FName> FamiliesA = GetClassSupportedWeaponFamilies(ClassTagA);
		const TArray<FName> FamiliesB = GetClassSupportedWeaponFamilies(ClassTagB);
		for (const FName& FamA : FamiliesA)
		{
			if (FamiliesB.Contains(FamA))
			{
				return true;
			}
		}
		return false;
	}

	static bool IsWeaponFamilySupportedByClass(const FGameplayTag& ClassTag, const FName& WeaponFamily)
	{
		const TArray<FName> Families = GetClassSupportedWeaponFamilies(ClassTag);
		return Families.Contains(WeaponFamily);
	}

	static TArray<FGameplayTag> GetAll16ClassTags()
	{
		return {
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Vanguard"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Templar"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Berserker"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.Swordmaster"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.DragonKnight"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Guard.VoidBlade"))),

			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Scout.Ranger"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Scout.Shadowblade"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Scout.PhantomStalker"))),

			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Caster.Arcanist"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Caster.Elementalist"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Caster.Chronomancer"))),

			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Faith.Acolyte"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Faith.Inquisitor"))),
			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Faith.Seraph"))),

			FGameplayTag::RequestGameplayTag(FName(TEXT("Class.Line.Apex.GodSlayer")))
		};
	}
};
