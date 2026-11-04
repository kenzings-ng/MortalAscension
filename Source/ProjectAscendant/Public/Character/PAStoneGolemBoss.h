// Copyright Project Ascendant. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/PABaseCharacter.h"
#include "World/PAMonsterLeashComponent.h"
#include "PAStoneGolemBoss.generated.h"

class UPAMonsterLeashComponent;
class UPAPostureSyncComponent;
class UPAThreatComponent;
class UPABossAIComponent;

/**
 * APAStoneGolemBoss
 *
 * Boss Stone Golem trong Project Ascendant (2.5D HD-2D Dark Fantasy Action RPG).
 * Kế thừa APABaseCharacter, tích hợp:
 *  - AC-1: PaperZD State Machine (FB_Golem_Idle, FB_Golem_Walk, FB_Golem_Slam, FB_Golem_Stagger, FB_Golem_Death @ 10 FPS).
 *  - AC-2: AI Aggro & Monster Leash Component (AggroRadius 800cm, LeashRadius 2500cm, tự động truy đuổi hoặc hồi vị trí).
 *  - AC-3: Kỹ năng Ground Slam AoE (Khoảng cách <= 250cm, phạm vi nện búa AoE 300cm gây 35 sát thương và 30 posture damage).
 */
UCLASS()
class PROJECTASCENDANT_API APAStoneGolemBoss : public APABaseCharacter
{
	GENERATED_BODY()

public:
	APAStoneGolemBoss(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	// -------------------------------------------------------------------------
	// AI Aggro & Combat Logic
	// -------------------------------------------------------------------------

	/** Cập nhật AI chu kỳ: tìm mục tiêu, truy đuổi và kích hoạt kỹ năng */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Boss|AI")
	void UpdateBossAI(float DeltaSeconds);

	/** Kiểm tra tính hợp lệ của mục tiêu giao tranh (chưa chết, không ở trong Sanctuary, trong phạm vi cho phép) */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Boss|AI")
	bool IsTargetCombatValid(AActor* TargetActor) const;

	/** Thử kích hoạt đòn đập đất Ground Slam */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Boss|Combat")
	bool TryExecuteGroundSlam();

	/** Thực thi sát thương AoE của cú đập Ground Slam (gọi từ AnimNotify hoặc logic server) */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Boss|Combat")
	void PerformSlamAoEDamage();

	/** Thiết lập mục tiêu truy đuổi */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Boss|AI")
	void SetTargetActor(AActor* NewTarget);

	/** Trả về mục tiêu hiện tại */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Boss|AI")
	AActor* GetTargetActor() const { return CurrentTarget.Get(); }

	/** Trả về Leash Component */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Boss|Leash")
	UPAMonsterLeashComponent* GetLeashComponent() const { return LeashComponent; }

	/** Trả về PostureSyncComponent quản lý vỡ thế Stagger */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Boss|Posture")
	UPAPostureSyncComponent* GetPostureSyncComponent() const { return PostureSyncComponent; }

	/** Trả về ThreatComponent quản lý bảng nộ khí */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Boss|Threat")
	UPAThreatComponent* GetThreatComponent() const { return ThreatComponent; }

	/** Trả về BossAIComponent */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Boss|AI")
	UPABossAIComponent* GetBossAIComponent() const { return BossAIComponent; }

	virtual void HandlePostureBroken(AActor* InstigatorActor) override;
	virtual void HandleOutOfHealth(AActor* InstigatorActor) override;
	virtual void HandleDamageReceived(AActor* InstigatorActor, float DamageAmount) override;
	virtual void HandlePostureDamageReceived(AActor* InstigatorActor, float PostureAmount) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Kiểm tra xem Boss có đang trong trạng thái Slam không */
	UFUNCTION(BlueprintPure, Category = "ProjectAscendant|Boss|Combat")
	bool IsPerformingSlam() const { return bIsPerformingSlam; }

	/** Hoàn tất chiêu Slam (reset cờ đánh) */
	UFUNCTION(BlueprintCallable, Category = "ProjectAscendant|Boss|Combat")
	void FinishGroundSlam();

	// -------------------------------------------------------------------------
	// Configuration Properties
	// -------------------------------------------------------------------------

	/** Bán kính phát hiện và kích hoạt Aggro (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Boss|AI")
	float AggroRadius;

	/** Tầm đánh kích hoạt Ground Slam (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Boss|Combat")
	float AttackRange;

	/** Thời gian hồi chiêu Ground Slam (giây) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Boss|Combat")
	float SlamCooldown;

	/** Bán kính sát thương lan AoE của Ground Slam (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Boss|Combat")
	float SlamAoERadius;

	/** Sát thương vật lý cơ bản của Ground Slam */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Boss|Combat")
	float SlamBaseDamage;

	/** Sát thương phá vỡ Posture của Ground Slam */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Boss|Combat")
	float SlamPostureDamage;

	/** Tốc độ di chuyển khi đi bộ tuần tra / truy đuổi (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectAscendant|Boss|Locomotion")
	float BossWalkSpeed;

	/** Xử lý sự kiện trạng thái Leash thay đổi */
	UFUNCTION()
	void HandleLeashStateChanged(bool bIsLeashing);

	/** Xử lý sự kiện hoàn tất Leash quay về điểm ban đầu */
	UFUNCTION()
	void HandleLeashCompleted();

protected:

	/** Hết cooldown Slam */
	UFUNCTION()
	void ResetSlamCooldown();

	/** Component quản lý biên giới quay đầu Leash */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectAscendant|Boss|Components")
	TObjectPtr<UPAMonsterLeashComponent> LeashComponent;

	/** Component quản lý đồng bộ trạng thái vỡ thế Stagger và cửa sổ kết liễu Finisher */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectAscendant|Boss|Components")
	TObjectPtr<UPAPostureSyncComponent> PostureSyncComponent;

	/** Component quản lý bảng nộ khí Threat Table theo thẩm quyền Server */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectAscendant|Boss|Components")
	TObjectPtr<UPAThreatComponent> ThreatComponent;

	/** Component máy trạng thái AI Boss cấp cao */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectAscendant|Boss|Components")
	TObjectPtr<UPABossAIComponent> BossAIComponent;

	/** Mục tiêu hiện tại đang nhắm đến */
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> CurrentTarget;

	/** Cờ trạng thái đang dập búa Slam */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectAscendant|Boss|State")
	bool bIsPerformingSlam;

	/** Cờ sẵn sàng thi triển Slam */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectAscendant|Boss|State")
	bool bCanSlam;

	/** Timer quản lý hồi chiêu Slam */
	FTimerHandle SlamCooldownTimerHandle;

	/** Timer tự động kết thúc chiêu Slam */
	FTimerHandle SlamDurationTimerHandle;
};
