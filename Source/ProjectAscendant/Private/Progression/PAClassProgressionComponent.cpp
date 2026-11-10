// Copyright Project Ascendant. All Rights Reserved.

#include "Progression/PAClassProgressionComponent.h"
#include "Progression/ClassPromotionScrollDefinition.h"
#include "Inventory/PAInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

UPAClassProgressionComponent::UPAClassProgressionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	PrimaryClassTag = FGameplayTag::EmptyTag;
	SecondaryClassTag = FGameplayTag::EmptyTag;
	bIsInCitadel = false;
}

void UPAClassProgressionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPAClassProgressionComponent, PrimaryClassTag);
	DOREPLIFETIME(UPAClassProgressionComponent, SecondaryClassTag);
	DOREPLIFETIME(UPAClassProgressionComponent, ProgressHistory);
	DOREPLIFETIME(UPAClassProgressionComponent, bIsInCitadel);
	DOREPLIFETIME(UPAClassProgressionComponent, OwnedQuestFlags);
}

EPAClassRank UPAClassProgressionComponent::GetPrimaryClassRank() const
{
	return FPAClassProgressionHelper::GetClassRank(PrimaryClassTag);
}

EPAClassRank UPAClassProgressionComponent::GetSecondaryClassRank() const
{
	return FPAClassProgressionHelper::GetClassRank(SecondaryClassTag);
}

EPAClassRank UPAClassProgressionComponent::GetEffectiveSecondaryRank() const
{
	if (!SecondaryClassTag.IsValid())
	{
		return EPAClassRank::None;
	}

	const EPAClassRank PriRank = GetPrimaryClassRank();
	const EPAClassRank SecRank = GetSecondaryClassRank();

	// Quy tắc A1 (Rank Capping): Rank_Sub <= Rank_Primary
	const uint8 ClampedVal = FMath::Min(static_cast<uint8>(SecRank), static_cast<uint8>(PriRank));
	return static_cast<EPAClassRank>(ClampedVal);
}

bool UPAClassProgressionComponent::IsClassUnlocked(const FGameplayTag& InClassTag) const
{
	const FPAClassProgressRecord* Rec = FindProgressRecord(InClassTag);
	return Rec != nullptr && Rec->bUnlocked;
}

int32 UPAClassProgressionComponent::GetClassLevel(const FGameplayTag& InClassTag) const
{
	const FPAClassProgressRecord* Rec = FindProgressRecord(InClassTag);
	return Rec ? Rec->ClassLevel : 0;
}

int32 UPAClassProgressionComponent::GetClassXP(const FGameplayTag& InClassTag) const
{
	const FPAClassProgressRecord* Rec = FindProgressRecord(InClassTag);
	return Rec ? Rec->ClassXP : 0;
}

bool UPAClassProgressionComponent::IsInCitadel() const
{
	if (bIsInCitadel)
	{
		return true;
	}

	// Kiểm tra thêm GameplayTag từ Owner Actor nếu có ASC
	if (const AActor* OwnerActor = GetOwner())
	{
		if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerActor))
		{
			if (const UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				const FGameplayTag SanctuaryTag = FGameplayTag::RequestGameplayTag(FName(TEXT("State.InSanctuary")), false);
				const FGameplayTag ZoneSanctuaryTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Zone.Sanctuary")), false);
				if ((SanctuaryTag.IsValid() && ASC->HasMatchingGameplayTag(SanctuaryTag)) ||
					(ZoneSanctuaryTag.IsValid() && ASC->HasMatchingGameplayTag(ZoneSanctuaryTag)))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void UPAClassProgressionComponent::SetInCitadel(bool bInCitadel)
{
	bIsInCitadel = bInCitadel;
}

bool UPAClassProgressionComponent::InitializeFoundationalClass(const FGameplayTag& InClassTag)
{
	if (!InClassTag.IsValid())
	{
		return false;
	}

	PrimaryClassTag = InClassTag;

	FPAClassProgressRecord* Rec = FindProgressRecord(InClassTag);
	if (!Rec)
	{
		FPAClassProgressRecord NewRec;
		NewRec.ClassTag = InClassTag;
		NewRec.ClassLevel = 1;
		NewRec.ClassXP = 0;
		NewRec.bUnlocked = true;
		ProgressHistory.Add(NewRec);
	}
	else
	{
		Rec->bUnlocked = true;
	}

	return true;
}

bool UPAClassProgressionComponent::SetSecondaryClass(const FGameplayTag& InClassTag)
{
	if (!InClassTag.IsValid())
	{
		SecondaryClassTag = FGameplayTag::EmptyTag;
		return true;
	}

	SecondaryClassTag = InClassTag;

	FPAClassProgressRecord* Rec = FindProgressRecord(InClassTag);
	if (!Rec)
	{
		FPAClassProgressRecord NewRec;
		NewRec.ClassTag = InClassTag;
		NewRec.ClassLevel = 1;
		NewRec.ClassXP = 0;
		NewRec.bUnlocked = true;
		ProgressHistory.Add(NewRec);
	}
	else
	{
		Rec->bUnlocked = true;
	}

	return true;
}

bool UPAClassProgressionComponent::SetPrimaryClass(const FGameplayTag& InClassTag)
{
	return InitializeFoundationalClass(InClassTag);
}

void UPAClassProgressionComponent::GrantCombatClassXP(int32 TotalCombatClassXP)
{
	if (TotalCombatClassXP <= 0)
	{
		return;
	}

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!PrimaryClassTag.IsValid())
	{
		return;
	}

	if (SecondaryClassTag.IsValid())
	{
		// Dual-Track split: 70% Primary, 30% Secondary (DECISIONS.md §4)
		const int32 PrimaryXP = FMath::Max(1, FMath::FloorToInt(static_cast<float>(TotalCombatClassXP) * 0.70f));
		const int32 SecondaryXP = TotalCombatClassXP - PrimaryXP;

		AddOrUpdateClassProgress(PrimaryClassTag, 0, PrimaryXP);
		AddOrUpdateClassProgress(SecondaryClassTag, 0, SecondaryXP);
	}
	else
	{
		// 100% nạp vào Class Chính khi chưa có Class Phụ
		AddOrUpdateClassProgress(PrimaryClassTag, 0, TotalCombatClassXP);
	}
}

bool UPAClassProgressionComponent::CanSwapPrimarySecondary() const
{
	FString DummyReason;
	return CanSwapPrimarySecondary(DummyReason);
}

bool UPAClassProgressionComponent::CanSwapPrimarySecondary(FString& OutFailReason) const
{
	// 1. Phải ở trong Tòa Thành / Sanctuary (Quy tắc A1)
	if (!IsInCitadel())
	{
		OutFailReason = TEXT("Must be in Citadel / Sanctuary to swap Primary and Secondary classes (Rule A1).");
		return false;
	}

	// 2. Class Phụ phải hợp lệ
	if (!SecondaryClassTag.IsValid())
	{
		OutFailReason = TEXT("Secondary class slot is empty.");
		return false;
	}

	// 3. Class Chính phải hợp lệ
	if (!PrimaryClassTag.IsValid())
	{
		OutFailReason = TEXT("Primary class slot is empty.");
		return false;
	}

	return true;
}

bool UPAClassProgressionComponent::SwapPrimarySecondaryClasses(EPAClassPromotionError& OutError)
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		OutError = EPAClassPromotionError::NotAuthoritative;
		return false;
	}

	FString FailReason;
	if (!CanSwapPrimarySecondary(FailReason))
	{
		OutError = !IsInCitadel() ? EPAClassPromotionError::NotInCitadel : EPAClassPromotionError::InvalidClassSlot;
		return false;
	}

	// Thực hiện hoán đổi
	const FGameplayTag Temp = PrimaryClassTag;
	PrimaryClassTag = SecondaryClassTag;
	SecondaryClassTag = Temp;

	OutError = EPAClassPromotionError::None;
	OnClassesSwapped.Broadcast(PrimaryClassTag, SecondaryClassTag);

	return true;
}

bool UPAClassProgressionComponent::IsSecondaryActiveSkillAllowed(FName AbilityWeaponFamily, bool bIsWeaponAgnostic) const
{
	if (!SecondaryClassTag.IsValid())
	{
		return false;
	}

	// Nếu cùng họ vũ khí (hoặc class hỗ trợ chung họ vũ khí như God Slayer hỗ trợ Blade & Polearm)
	if (FPAClassProgressionHelper::AreWeaponFamiliesCompatible(PrimaryClassTag, SecondaryClassTag))
	{
		// Nếu kỹ năng yêu cầu một họ vũ khí cụ thể, kiểm tra xem cả Class Chính và Class Phụ có hỗ trợ không
		if (AbilityWeaponFamily.IsNone() ||
			(FPAClassProgressionHelper::IsWeaponFamilySupportedByClass(PrimaryClassTag, AbilityWeaponFamily) &&
			 FPAClassProgressionHelper::IsWeaponFamilySupportedByClass(SecondaryClassTag, AbilityWeaponFamily)))
		{
			return true;
		}
	}

	// Quy tắc B1: Khác họ vũ khí -> Chỉ cho phép kỹ năng Không Phụ Thuộc Vũ Khí (Weapon-Agnostic)
	return bIsWeaponAgnostic;
}

EPAClassPromotionError UPAClassProgressionComponent::ExecutePromotion(
	const UClassPromotionScrollDefinition* ScrollDef,
	EPAClassSlot TargetSlot,
	const FGameplayTagContainer& CharacterFlags)
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		return EPAClassPromotionError::NotAuthoritative;
	}

	if (!ScrollDef || !ScrollDef->TargetClassTag.IsValid())
	{
		return EPAClassPromotionError::InvalidScroll;
	}

	// Kiểm tra vật phẩm quyển trục trong túi đồ nếu nhân vật có gắn InventoryComponent (ADR-0003, ROADMAP.md:83)
	UPAInventoryComponent* InventoryComp = GetOwner() ? GetOwner()->FindComponentByClass<UPAInventoryComponent>() : nullptr;
	if (InventoryComp && !ScrollDef->ScrollID.IsNone())
	{
		if (InventoryComp->GetItemCount(ScrollDef->ScrollID) < 1)
		{
			return EPAClassPromotionError::MissingScrollItem;
		}
	}

	// Kiểm tra điều kiện thăng chức
	if (ScrollDef->bRequiresDualSlotCondition)
	{
		// Apex requirement: Phải thỏa mãn cả 2 slot
		const int32 PriLevel = GetClassLevel(PrimaryClassTag);
		const int32 SecLevel = GetClassLevel(SecondaryClassTag);

		if (!ScrollDef->CanPromoteDualSlot(PrimaryClassTag, PriLevel, SecondaryClassTag, SecLevel, CharacterFlags))
		{
			return EPAClassPromotionError::ApexDualSlotPrerequisiteNotMet;
		}
	}
	else
	{
		const FGameplayTag CandidateTag = (TargetSlot == EPAClassSlot::Primary) ? PrimaryClassTag : SecondaryClassTag;
		const int32 CandidateLevel = GetClassLevel(CandidateTag);

		// Nếu slot phụ đang rỗng và scroll không yêu cầu class nguồn cụ thể (AllowedSourceClassTags rỗng) và RequiredMinRank <= 1
		const bool bEmptySecondaryAllowed = (TargetSlot == EPAClassSlot::Secondary && !SecondaryClassTag.IsValid() && ScrollDef->AllowedSourceClassTags.Num() == 0 && ScrollDef->RequiredMinRank <= 1);

		if (!bEmptySecondaryAllowed && !ScrollDef->CanPromote(CandidateTag, CandidateLevel, CharacterFlags))
		{
			if (ScrollDef->RequiredQuestFlag.IsValid() && !CharacterFlags.HasTag(ScrollDef->RequiredQuestFlag))
			{
				return EPAClassPromotionError::MissingQuestFlag;
			}
			return EPAClassPromotionError::RequirementsNotMet;
		}
	}

	// Tiêu hao quyển trục khỏi túi đồ (Server-Authoritative)
	if (InventoryComp && !ScrollDef->ScrollID.IsNone() && InventoryComp->GetItemCount(ScrollDef->ScrollID) >= 1)
	{
		InventoryComp->ConsumeItemQuantity(ScrollDef->ScrollID, 1);
	}

	// Ghi nhận thăng chức
	const FGameplayTag NewClassTag = ScrollDef->TargetClassTag;
	if (TargetSlot == EPAClassSlot::Primary)
	{
		PrimaryClassTag = NewClassTag;
	}
	else
	{
		SecondaryClassTag = NewClassTag;
	}

	// Mở khóa và bảo toàn tiến trình trong lịch sử
	FPAClassProgressRecord* Rec = FindProgressRecord(NewClassTag);
	if (!Rec)
	{
		FPAClassProgressRecord NewRec;
		NewRec.ClassTag = NewClassTag;
		NewRec.ClassLevel = 1;
		NewRec.ClassXP = 0;
		NewRec.bUnlocked = true;
		ProgressHistory.Add(NewRec);
	}
	else
	{
		Rec->bUnlocked = true;
	}

	OnClassPromoted.Broadcast(TargetSlot, NewClassTag);
	return EPAClassPromotionError::None;
}

void UPAClassProgressionComponent::ServerPromoteClass_Implementation(
	const UClassPromotionScrollDefinition* ScrollDef,
	EPAClassSlot TargetSlot)
{
	const EPAClassPromotionError Error = ExecutePromotion(ScrollDef, TargetSlot, OwnedQuestFlags);
	if (Error != EPAClassPromotionError::None)
	{
		OnClassPromotionFailed.Broadcast(TargetSlot, Error);
	}
}

bool UPAClassProgressionComponent::ServerPromoteClass_Validate(
	const UClassPromotionScrollDefinition* ScrollDef,
	EPAClassSlot TargetSlot)
{
	return ScrollDef != nullptr && (TargetSlot == EPAClassSlot::Primary || TargetSlot == EPAClassSlot::Secondary);
}

void UPAClassProgressionComponent::AddQuestFlag(const FGameplayTag& FlagTag)
{
	if (FlagTag.IsValid())
	{
		OwnedQuestFlags.AddTag(FlagTag);
	}
}

bool UPAClassProgressionComponent::IsPrimaryActiveSlot(FName SlotName) const
{
	return SlotName == FName(TEXT("Q")) || SlotName == FName(TEXT("E")) || SlotName == FName(TEXT("R"));
}

bool UPAClassProgressionComponent::IsSecondaryActiveSlot(FName SlotName) const
{
	return SlotName == FName(TEXT("F"));
}

EPAClassSlot UPAClassProgressionComponent::GetPassiveSlotOwner(FName SlotName) const
{
	if (SlotName == FName(TEXT("P3")))
	{
		return EPAClassSlot::Secondary;
	}
	return EPAClassSlot::Primary; // P1, P2
}

void UPAClassProgressionComponent::OnRep_PrimaryClassTag()
{
}

void UPAClassProgressionComponent::OnRep_SecondaryClassTag()
{
}

void UPAClassProgressionComponent::OnRep_ProgressHistory()
{
}

FPAClassProgressRecord* UPAClassProgressionComponent::FindProgressRecord(const FGameplayTag& InTag)
{
	for (FPAClassProgressRecord& Rec : ProgressHistory)
	{
		if (Rec.ClassTag == InTag)
		{
			return &Rec;
		}
	}
	return nullptr;
}

const FPAClassProgressRecord* UPAClassProgressionComponent::FindProgressRecord(const FGameplayTag& InTag) const
{
	for (const FPAClassProgressRecord& Rec : ProgressHistory)
	{
		if (Rec.ClassTag == InTag)
		{
			return &Rec;
		}
	}
	return nullptr;
}

void UPAClassProgressionComponent::AddOrUpdateClassProgress(const FGameplayTag& InTag, int32 LevelDelta, int32 XPDelta)
{
	FPAClassProgressRecord* Rec = FindProgressRecord(InTag);
	if (!Rec)
	{
		FPAClassProgressRecord NewRec;
		NewRec.ClassTag = InTag;
		NewRec.ClassLevel = 1;
		NewRec.ClassXP = 0;
		NewRec.bUnlocked = true;
		ProgressHistory.Add(NewRec);
		Rec = &ProgressHistory.Last();
	}

	Rec->ClassXP += XPDelta;

	// Xử lý lên cấp Class Level (tối đa 20)
	while (Rec->ClassLevel < FPAClassProgressRecord::MaxClassLevel)
	{
		const int32 XPNeeded = FPAClassProgressRecord::GetXPRequiredForClassLevel(Rec->ClassLevel);
		if (Rec->ClassXP >= XPNeeded)
		{
			Rec->ClassXP -= XPNeeded;
			Rec->ClassLevel++;
		}
		else
		{
			break;
		}
	}

	OnClassXPGained.Broadcast(InTag, XPDelta, Rec->ClassLevel);
}
