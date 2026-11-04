// Copyright Project Ascendant. All Rights Reserved.

#include "Character/PAStoneGolemBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Combat/AscendantAttributeSet.h"
#include "Combat/PAPostureSyncComponent.h"
#include "Combat/PAThreatComponent.h"
#include "AI/PABossAIComponent.h"
#include "Controller/PABasePlayerController.h"
#include "UI/PAGameplayHUDWidget.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "TimerManager.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "PaperZDAnimationComponent.h"
#include "PaperZDAnimInstance.h"
#include "Animation/PAPaperZDAnimInstance.h"
#include "Engine/Blueprint.h"

APAStoneGolemBoss::APAStoneGolemBoss(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// DECISIONS.md §11: Quái vật / Boss / NPC dùng GAS replication mode Minimal
	// (client chỉ nhận Attributes + GameplayTags + GameplayCues, không nhận GE spec).
	// APABaseCharacter mặc định Mixed cho nhân vật người chơi.
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	}

	// Tạo Leash Component và cấu hình thông số theo AC-2
	LeashComponent = CreateDefaultSubobject<UPAMonsterLeashComponent>(TEXT("LeashComponent"));
	if (LeashComponent)
	{
		LeashComponent->SetLeashDistance(2500.0f);
		LeashComponent->SetLeashSpeedMultiplier(1.5f);
	}

	// Khởi tạo các Component quản lý Boss theo chuẩn M6 & GDD
	PostureSyncComponent = CreateDefaultSubobject<UPAPostureSyncComponent>(TEXT("PostureSyncComponent"));
	ThreatComponent = CreateDefaultSubobject<UPAThreatComponent>(TEXT("ThreatComponent"));
	BossAIComponent = CreateDefaultSubobject<UPABossAIComponent>(TEXT("BossAIComponent"));

	// Cấu hình thông số mặc định cho Stone Golem Boss
	AggroRadius = 800.0f;
	AttackRange = 250.0f;
	SlamCooldown = 3.0f;
	SlamAoERadius = 300.0f;
	SlamBaseDamage = 35.0f;
	SlamPostureDamage = 30.0f;
	BossWalkSpeed = 220.0f;

	bIsPerformingSlam = false;
	bCanSlam = true;

	SetBaseMoveSpeed(BossWalkSpeed);
}

void APAStoneGolemBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (LeashComponent)
	{
		LeashComponent->OnLeashStateChanged.AddUniqueDynamic(this, &APAStoneGolemBoss::HandleLeashStateChanged);
		LeashComponent->OnLeashCompleted.AddUniqueDynamic(this, &APAStoneGolemBoss::HandleLeashCompleted);
	}
}

void APAStoneGolemBoss::BeginPlay()
{
	Super::BeginPlay();

	SetBaseMoveSpeed(BossWalkSpeed);

	// Nạp Sprite Flipbook mặc định cho Boss Stone Golem
	if (SpriteComponent)
	{
		if (UPaperFlipbook* GolemIdleFb = Cast<UPaperFlipbook>(StaticLoadObject(UPaperFlipbook::StaticClass(), nullptr, TEXT("/Game/art/characters/boss/flipbooks/FB_Golem_Idle.FB_Golem_Idle"))))
		{
			SpriteComponent->SetFlipbook(GolemIdleFb);
		}
	}

	// Nạp PaperZD Animation Blueprint cho Boss Stone Golem (hỗ trợ cả ABP_StoneGolem và ABP_Boss)
	if (PaperZDAnimComponent)
	{
		if (!PaperZDAnimComponent->GetAnimInstanceClass())
		{
			PaperZDAnimComponent->SetAnimInstanceClass(UPAPaperZDAnimInstance::StaticClass());
		}

		UClass* BossAnimClass = StaticLoadClass(UPaperZDAnimInstance::StaticClass(), nullptr, TEXT("/Game/art/characters/boss/anim/ABP_StoneGolem.ABP_StoneGolem_C"), nullptr, LOAD_Quiet);
		if (!BossAnimClass)
		{
			BossAnimClass = StaticLoadClass(UPaperZDAnimInstance::StaticClass(), nullptr, TEXT("/Game/art/characters/boss/anim/ABP_Boss.ABP_Boss_C"), nullptr, LOAD_Quiet);
		}
		if (!BossAnimClass)
		{
			if (UBlueprint* BossBP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, TEXT("/Game/art/characters/boss/anim/ABP_StoneGolem.ABP_StoneGolem"), nullptr, LOAD_Quiet)))
			{
				BossAnimClass = BossBP->GeneratedClass;
			}
			else if (UBlueprint* LegacyBossBP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, TEXT("/Game/art/characters/boss/anim/ABP_Boss.ABP_Boss"), nullptr, LOAD_Quiet)))
			{
				BossAnimClass = LegacyBossBP->GeneratedClass;
			}
		}
		if (BossAnimClass && BossAnimClass->IsChildOf(UPaperZDAnimInstance::StaticClass()))
		{
			PaperZDAnimComponent->SetAnimInstanceClass(BossAnimClass);
		}
	}

	if (LeashComponent)
	{
		LeashComponent->SetSpawnOrigin(GetActorLocation());
	}

	if (ThreatComponent)
	{
		ThreatComponent->SetSpawnOrigin(GetActorLocation());
	}

	if (BossAIComponent)
	{
		const float MaxHP = (AttributeSet && AttributeSet->GetMaxHealth() > 0.0f) ? AttributeSet->GetMaxHealth() : 10000.0f;
		BossAIComponent->InitializeBoss(MaxHP);
	}
}

void APAStoneGolemBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LeashComponent)
	{
		LeashComponent->OnLeashStateChanged.RemoveDynamic(this, &APAStoneGolemBoss::HandleLeashStateChanged);
		LeashComponent->OnLeashCompleted.RemoveDynamic(this, &APAStoneGolemBoss::HandleLeashCompleted);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SlamDurationTimerHandle);
		World->GetTimerManager().ClearTimer(SlamCooldownTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void APAStoneGolemBoss::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		UpdateBossAI(DeltaSeconds);
	}
}

bool APAStoneGolemBoss::IsTargetCombatValid(AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return false;
	}

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		static const FGameplayTag TagDead = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"), false);
		static const FGameplayTag TagInSanctuary = FGameplayTag::RequestGameplayTag(TEXT("State.InSanctuary"), false);
		if ((TagDead.IsValid() && TargetASC->HasMatchingGameplayTag(TagDead)) ||
			(TagInSanctuary.IsValid() && TargetASC->HasMatchingGameplayTag(TagInSanctuary)))
		{
			return false;
		}
	}

	if (const APABaseCharacter* TargetChar = Cast<APABaseCharacter>(TargetActor))
	{
		if (TargetChar->GetAttributeSet() && TargetChar->GetAttributeSet()->GetHealth() <= 0.0f)
		{
			return false;
		}
	}

	const float DistSq = FVector::DistSquared2D(GetActorLocation(), TargetActor->GetActorLocation());
	if (DistSq > FMath::Square(AggroRadius * 1.5f))
	{
		return false;
	}

	return true;
}

void APAStoneGolemBoss::UpdateBossAI(float DeltaSeconds)
{
	// 0. Nếu Boss đã chết, không thực hiện hành động
	const bool bDead = (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead"), false))) ||
		(AttributeSet && AttributeSet->GetHealth() <= 0.0f);
	if (bDead)
	{
		return;
	}

	// 1. Nếu Boss đang bị vỡ thế Stagger (choáng 3.0s), dừng chuyển động và không tấn công
	if (PostureSyncComponent && PostureSyncComponent->IsStaggered())
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}
		return;
	}

	// 2. Nếu đang trong trạng thái hồi vị trí (Leash Resetting), bỏ qua truy đuổi
	if (LeashComponent && LeashComponent->IsLeashResetting())
	{
		SetTargetActor(nullptr);
		return;
	}

	// 3. Nếu đang tung đòn Ground Slam, đứng yên dập đất
	if (bIsPerformingSlam)
	{
		return;
	}

	// 4. Ưu tiên mục tiêu có Threat cao nhất từ ThreatComponent (loại bỏ các mục tiêu stale/chết/Sanctuary)
	if (ThreatComponent)
	{
		ThreatComponent->EvaluateThreatRetargeting();
		AActor* AggroTarget = ThreatComponent->GetCurrentAggroTarget();
		while (AggroTarget && !IsTargetCombatValid(AggroTarget))
		{
			ThreatComponent->RemoveCombatant(AggroTarget);
			AggroTarget = ThreatComponent->GetCurrentAggroTarget();
		}

		if (AggroTarget && AggroTarget != CurrentTarget.Get())
		{
			SetTargetActor(AggroTarget);
		}
	}

	// 5. Nếu chưa có mục tiêu, quét tìm người chơi gần nhất trong bán kính AggroRadius (hỗ trợ Dedicated Server)
	if (!CurrentTarget.IsValid())
	{
		float ClosestDistSq = FMath::Square(AggroRadius);
		AActor* BestTarget = nullptr;

		if (UWorld* World = GetWorld())
		{
			for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				if (APlayerController* PC = Iterator->Get())
				{
					if (APawn* PlayerPawn = PC->GetPawn())
					{
						if (PlayerPawn != this && IsTargetCombatValid(PlayerPawn))
						{
							const float DistSq = FVector::DistSquared2D(GetActorLocation(), PlayerPawn->GetActorLocation());
							if (DistSq <= ClosestDistSq)
							{
								ClosestDistSq = DistSq;
								BestTarget = PlayerPawn;
							}
						}
					}
				}
			}
		}

		if (BestTarget)
		{
			SetTargetActor(BestTarget);
			if (ThreatComponent)
			{
				ThreatComponent->AddThreat(BestTarget, 10.0f, EPAThreatSource::Damage);
			}
		}
	}

	// 6. Nếu có mục tiêu hợp lệ, kiểm tra lại tính hợp lệ
	if (CurrentTarget.IsValid())
	{
		if (!IsTargetCombatValid(CurrentTarget.Get()))
		{
			if (ThreatComponent)
			{
				ThreatComponent->RemoveCombatant(CurrentTarget.Get());
			}
			SetTargetActor(nullptr);
			return;
		}

		// Hướng quay mặt ngắm về phía mục tiêu
		const FVector ToTarget = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
		UpdateAimDirection(ToTarget);

		const float Dist = FVector::Dist2D(GetActorLocation(), CurrentTarget->GetActorLocation());
		// Nếu đã áp sát trong tầm đánh (AttackRange <= 250cm)
		if (Dist <= AttackRange)
		{
			TryExecuteGroundSlam();
		}
		else
		{
			// Di chuyển áp sát mục tiêu
			AddMovementInput(ToTarget, 1.0f);
		}
	}
}

bool APAStoneGolemBoss::TryExecuteGroundSlam()
{
	if (!bCanSlam || bIsPerformingSlam)
	{
		return false;
	}

	bIsPerformingSlam = true;
	bCanSlam = false;

	// Gắn tag State.Attacking để PaperZD AnimBP kích hoạt chuyển sang state Slam
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Attacking")));
	}

	// Dừng quán tính di chuyển khi chuẩn bị dập
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
	}

	// Hẹn giờ kích hoạt gây sát thương AoE tại thời điểm dập búa (0.4s)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SlamDurationTimerHandle,
			this,
			&APAStoneGolemBoss::PerformSlamAoEDamage,
			0.4f,
			false
		);
	}

	return true;
}

void APAStoneGolemBoss::PerformSlamAoEDamage()
{
	if (!HasAuthority())
	{
		return;
	}

	// Guard: Nếu đòn đánh đã bị hủy hoặc Boss không còn ở trạng thái tấn công hợp lệ (AC-4 & Review Fix)
	const bool bDead = (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead"), false))) ||
		(AttributeSet && AttributeSet->GetHealth() <= 0.0f);
	const bool bStaggered = (PostureSyncComponent && PostureSyncComponent->IsStaggered());
	const bool bLeashResetting = (LeashComponent && LeashComponent->IsLeashResetting());

	if (!bIsPerformingSlam || bDead || bStaggered || bLeashResetting)
	{
		FinishGroundSlam();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		FinishGroundSlam();
		return;
	}

	// Quét vòng tròn AoE bán kính 300cm quanh vị trí của Boss
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape AoESphere = FCollisionShape::MakeSphere(SlamAoERadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHit = World->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		AoESphere,
		QueryParams
	);

	if (bHit)
	{
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			AActor* HitActor = Overlap.GetActor();
			if (HitActor && HitActor != this)
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
				{
					static const FGameplayTag TagInvuln = FGameplayTag::RequestGameplayTag(TEXT("State.Invulnerable"), false);
					static const FGameplayTag TagSanctuary = FGameplayTag::RequestGameplayTag(TEXT("State.InSanctuary"), false);
					if ((TagInvuln.IsValid() && TargetASC->HasMatchingGameplayTag(TagInvuln)) ||
						(TagSanctuary.IsValid() && TargetASC->HasMatchingGameplayTag(TagSanctuary)))
					{
						continue; // Bỏ qua nếu mục tiêu đang có I-frame né lướt hoặc ở trong vùng an toàn
					}

					// 1. Áp dụng sát thương máu lên mục tiêu qua GameplayEffect chuẩn (Server-Authoritative)
					UGameplayEffect* DamageEffect = NewObject<UGameplayEffect>(GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UGameplayEffect::StaticClass(), TEXT("GE_BossSlamDamage")));
					if (DamageEffect)
					{
						DamageEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
						FGameplayModifierInfo ModInfo;
						ModInfo.Attribute = UAscendantAttributeSet::GetHealthAttribute();
						ModInfo.ModifierOp = EGameplayModOp::Additive;
						ModInfo.ModifierMagnitude = FScalableFloat(-SlamBaseDamage);
						DamageEffect->Modifiers.Add(ModInfo);

						FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
						ContextHandle.AddSourceObject(this);
						TargetASC->ApplyGameplayEffectToSelf(DamageEffect, 1.0f, ContextHandle);
					}

					// 2. Áp dụng sát thương Posture (AC-3: 30 Posture damage)
					UGameplayEffect* PostureEffect = NewObject<UGameplayEffect>(GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UGameplayEffect::StaticClass(), TEXT("GE_BossSlamPostureDamage")));
					if (PostureEffect)
					{
						PostureEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
						FGameplayModifierInfo ModInfo;
						ModInfo.Attribute = UAscendantAttributeSet::GetPostureAttribute();
						ModInfo.ModifierOp = EGameplayModOp::Additive;
						ModInfo.ModifierMagnitude = FScalableFloat(SlamPostureDamage);
						PostureEffect->Modifiers.Add(ModInfo);

						FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
						ContextHandle.AddSourceObject(this);
						TargetASC->ApplyGameplayEffectToSelf(PostureEffect, 1.0f, ContextHandle);
					}
				}
				else
				{
					// Fallback cho Actor không có GAS
					UGameplayStatics::ApplyDamage(
						HitActor,
						SlamBaseDamage,
						GetController(),
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}
	}

	// Kết thúc chiêu sau 0.2s nữa
	World->GetTimerManager().SetTimer(
		SlamDurationTimerHandle,
		this,
		&APAStoneGolemBoss::FinishGroundSlam,
		0.2f,
		false
	);
}

void APAStoneGolemBoss::FinishGroundSlam()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SlamDurationTimerHandle);
	}

	bIsPerformingSlam = false;

	// Gỡ tag State.Attacking
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Attacking")));
	}

	// Đặt hẹn giờ hồi chiêu SlamCooldown (3.0s)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SlamCooldownTimerHandle,
			this,
			&APAStoneGolemBoss::ResetSlamCooldown,
			SlamCooldown,
			false
		);
	}
}

void APAStoneGolemBoss::ResetSlamCooldown()
{
	bCanSlam = true;
}

void APAStoneGolemBoss::SetTargetActor(AActor* NewTarget)
{
	if (CurrentTarget.Get() == NewTarget)
	{
		return;
	}

	// Ẩn Boss HUD trên mục tiêu cũ
	if (CurrentTarget.IsValid())
	{
		if (APABaseCharacter* OldChar = Cast<APABaseCharacter>(CurrentTarget.Get()))
		{
			if (APABasePlayerController* OldPC = Cast<APABasePlayerController>(OldChar->GetController()))
			{
				if (OldPC->GetGameplayHUDWidget())
				{
					OldPC->GetGameplayHUDWidget()->HideBossHUD();
				}
			}
		}
	}

	CurrentTarget = NewTarget;

	// Hiển thị Boss HUD trên mục tiêu mới
	if (NewTarget)
	{
		if (APABaseCharacter* NewChar = Cast<APABaseCharacter>(NewTarget))
		{
			if (APABasePlayerController* NewPC = Cast<APABasePlayerController>(NewChar->GetController()))
			{
				if (NewPC->GetGameplayHUDWidget() && !NewPC->GetGameplayHUDWidget()->IsBossHUDVisible())
				{
					static const TArray<FName> BossParts = { FName(TEXT("Core")), FName(TEXT("Horn")), FName(TEXT("Arm_Left")), FName(TEXT("Arm_Right")) };
					NewPC->GetGameplayHUDWidget()->ShowBossHUD(this, TEXT("Ancient Stone Golem"), 10, BossParts);
				}
			}
		}
	}
}

void APAStoneGolemBoss::HandleLeashStateChanged(bool bIsLeashing)
{
	if (bIsLeashing)
	{
		if (bIsPerformingSlam)
		{
			FinishGroundSlam();
		}

		SetTargetActor(nullptr);

		if (ThreatComponent)
		{
			ThreatComponent->TriggerLeashReset();
		}
	}
}

void APAStoneGolemBoss::HandleLeashCompleted()
{
	SetTargetActor(nullptr);
	if (ThreatComponent)
	{
		ThreatComponent->CompleteLeashReset();
	}
}

void APAStoneGolemBoss::HandlePostureBroken(AActor* InstigatorActor)
{
	Super::HandlePostureBroken(InstigatorActor);

	if (bIsPerformingSlam)
	{
		FinishGroundSlam();
	}

	if (AbilitySystemComponent)
	{
		static const FGameplayTag TagStaggered = FGameplayTag::RequestGameplayTag(TEXT("State.Staggered"), false);
		if (TagStaggered.IsValid())
		{
			AbilitySystemComponent->AddLooseGameplayTag(TagStaggered);
		}
	}

	if (PostureSyncComponent)
	{
		APABaseCharacter* InstigatorChar = Cast<APABaseCharacter>(InstigatorActor);
		const float MaxHP = (AttributeSet && AttributeSet->GetMaxHealth() > 0.0f) ? AttributeSet->GetMaxHealth() : 10000.0f;
		PostureSyncComponent->NotifyPostureBroken(InstigatorChar, FGuid(), MaxHP);
	}
}

void APAStoneGolemBoss::HandleOutOfHealth(AActor* InstigatorActor)
{
	Super::HandleOutOfHealth(InstigatorActor);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SlamDurationTimerHandle);
		World->GetTimerManager().ClearTimer(SlamCooldownTimerHandle);
	}

	if (bIsPerformingSlam)
	{
		FinishGroundSlam();
	}

	SetTargetActor(nullptr);

	if (BossAIComponent)
	{
		BossAIComponent->SetBossHealth(0.0f);
	}

	if (ThreatComponent)
	{
		ThreatComponent->ClearThreatTable();
	}
}

void APAStoneGolemBoss::HandleDamageReceived(AActor* InstigatorActor, float DamageAmount)
{
	Super::HandleDamageReceived(InstigatorActor, DamageAmount);

	if (DamageAmount > 0.0f && InstigatorActor && ThreatComponent)
	{
		ThreatComponent->AddThreat(InstigatorActor, DamageAmount, EPAThreatSource::Damage);
	}

	if (BossAIComponent && AttributeSet)
	{
		BossAIComponent->SetBossHealth(AttributeSet->GetHealth());
	}
}

void APAStoneGolemBoss::HandlePostureDamageReceived(AActor* InstigatorActor, float PostureAmount)
{
	Super::HandlePostureDamageReceived(InstigatorActor, PostureAmount);

	if (PostureAmount > 0.0f && InstigatorActor && ThreatComponent)
	{
		ThreatComponent->AddThreat(InstigatorActor, PostureAmount, EPAThreatSource::PostureDamage);
	}
}

float APAStoneGolemBoss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Nếu không có AbilitySystemComponent để xử lý qua PostGameplayEffectExecute (ví dụ standalone mock)
	if (ActualDamage > 0.0f && !AbilitySystemComponent)
	{
		AActor* ThreatActor = DamageCauser ? DamageCauser : (EventInstigator ? EventInstigator->GetPawn() : nullptr);
		if (ThreatActor && ThreatComponent)
		{
			ThreatComponent->AddThreat(ThreatActor, ActualDamage, EPAThreatSource::Damage);
		}

		if (BossAIComponent)
		{
			BossAIComponent->ApplyBossDamage(ActualDamage);
		}
	}

	return ActualDamage;
}
