// Copyright Project Ascendant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crafting/PABlacksmithTypes.h"
#include "PABlacksmithForge.generated.h"

class UPABlacksmithComponent;
class UStaticMeshComponent;

/**
 * APABlacksmithForge
 *
 * Actor Lò Rèn / Đe Rèn trong thế giới Project Ascendant.
 * Chứa UPABlacksmithComponent điều hành các nghiệp vụ:
 * Sửa chữa, Phân rã, Cường hóa (+0 -> +10), Khảm ngọc, Rèn Boss Soul.
 *
 * Tham chiếu GDD: design/gdd/blacksmithing-system.md
 * DECISIONS.md §13: Bán kính tương tác với merchant/forge: 300cm.
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTASCENDANT_API APABlacksmithForge : public AActor
{
	GENERATED_BODY()

public:
	APABlacksmithForge();

	virtual void BeginPlay() override;

	/** Trả về Blacksmith Component quản lý nghiệp vụ rèn */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Forge")
	UPABlacksmithComponent* GetBlacksmithComponent() const { return BlacksmithComp; }

	/** Bậc lò rèn hiện tại */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Forge")
	EPABlacksmithTier ForgeTier = EPABlacksmithTier::Tier1_Outpost;

	/** Tên hiển thị của Lò Rèn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Forge")
	FString ForgeDisplayName = TEXT("Tier 1 Outpost Forge");

	/** Bán kính tương tác tối đa (300cm - DECISIONS.md §13) */
	static constexpr float kMaxInteractionDistance = 300.0f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectAscendant|Forge")
	TObjectPtr<UPABlacksmithComponent> BlacksmithComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectAscendant|Forge")
	TObjectPtr<UStaticMeshComponent> ForgeMeshComp;
};
