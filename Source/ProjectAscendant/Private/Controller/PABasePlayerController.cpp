// Copyright Project Ascendant. All Rights Reserved.

#include "Controller/PABasePlayerController.h"
#include "Character/PABaseCharacter.h"
#include "Controller/PAIsometricMovementMath.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/MeshComponent.h"
#include "Engine/World.h"
#include "UI/PALoginWidget.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "Combat/PAGameplayAbility_MeleeAttack.h"
#include "Combat/PAGameplayAbility_Dash.h"
#include "Combat/PAGameplayAbility_Finisher.h"
#include "Network/PAServiceRequestComponent.h"
#include "Network/PAServerRequestValidation.h"
#include "UI/PAGameplayHUDWidget.h"
#include "UI/PAFloatingCombatTextComponent.h"
#include "Economy/PAMerchantComponent.h"
#include "Economy/PAWanderingSmuggler.h"
#include "Crafting/PABlacksmithComponent.h"
#include "Crafting/PABlacksmithForge.h"
#include "Inventory/PAInventoryComponent.h"
#include "Economy/PACurrencyComponent.h"
#include "Account/PAAccountSubsystem.h"
#include "EngineUtils.h"

APABasePlayerController::APABasePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Bật PlayerTick trên máy local của người chơi để chiếu tia con trỏ chuột và tính camera (Guardrail: <= 0.2ms)
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	LoginWidgetClass = UPALoginWidget::StaticClass();
	GameplayHUDWidgetClass = UPAGameplayHUDWidget::StaticClass();

	ServiceRequestComponent = CreateDefaultSubobject<UPAServiceRequestComponent>(TEXT("ServiceRequestComponent"));
	FloatingCombatTextComponent = CreateDefaultSubobject<UPAFloatingCombatTextComponent>(TEXT("FloatingCombatTextComponent"));
}

void APABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// Tầng Presentation: Chỉ khởi tạo giao diện trên client cục bộ khi chưa đăng nhập (Dedicated Server không chạm vào Viewport/UI)
	if (IsLocalController() && LoginWidgetClass)
	{
		const UPAAccountSubsystem* AccountSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UPAAccountSubsystem>() : nullptr;
		const bool bAlreadyLoggedIn = AccountSubsystem && AccountSubsystem->IsLoggedIn();

		if (!bAlreadyLoggedIn)
		{
			ActiveLoginWidget = CreateWidget<UPALoginWidget>(this, LoginWidgetClass);
			if (ActiveLoginWidget)
			{
				ActiveLoginWidget->AddToViewport(100);
				FInputModeGameAndUI InputMode;
				InputMode.SetWidgetToFocus(ActiveLoginWidget->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(InputMode);
				bShowMouseCursor = true;
			}
		}
		else
		{
			SetupGameplayHUD();
		}
	}
}

void APABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (IsLocalController())
	{
		SetupGameplayHUD();
	}
}

void APABasePlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);
	if (IsLocalController())
	{
		SetupGameplayHUD();

		// Synchronize client-selected CharacterClass from UPAAccountSubsystem to dedicated server
		if (const UGameInstance* GI = GetGameInstance())
		{
			if (const UPAAccountSubsystem* AccSub = GI->GetSubsystem<UPAAccountSubsystem>())
			{
				const FName SelectedClassTag = AccSub->GetSelectedCharacterClass();
				EPACharacterClass SelectedClass = EPACharacterClass::Vanguard;
				if (SelectedClassTag == FName(TEXT("Class.Line.Scout.Ranger")))
				{
					SelectedClass = EPACharacterClass::Ranger;
				}
				else if (SelectedClassTag == FName(TEXT("Class.Line.Caster.Arcanist")))
				{
					SelectedClass = EPACharacterClass::Arcanist;
				}
				Server_SetCharacterClass(SelectedClass);
			}
		}
	}
}

bool APABasePlayerController::Server_SetCharacterClass_Validate(EPACharacterClass NewClass)
{
	return NewClass == EPACharacterClass::Vanguard ||
	       NewClass == EPACharacterClass::Ranger ||
	       NewClass == EPACharacterClass::Arcanist;
}

void APABasePlayerController::Server_SetCharacterClass_Implementation(EPACharacterClass NewClass)
{
	if (APABaseCharacter* BaseChar = Cast<APABaseCharacter>(GetPawn()))
	{
		BaseChar->SetCharacterClass(NewClass);
	}
}

void APABasePlayerController::SetupGameplayHUD()
{
	if (!IsLocalController())
	{
		return;
	}

	if (!ActiveGameplayHUDWidget && GameplayHUDWidgetClass)
	{
		ActiveGameplayHUDWidget = CreateWidget<UPAGameplayHUDWidget>(this, GameplayHUDWidgetClass);
		if (ActiveGameplayHUDWidget)
		{
			ActiveGameplayHUDWidget->AddToViewport(10);
		}
	}

	if (ActiveGameplayHUDWidget)
	{
		ActiveGameplayHUDWidget->InitializeHUD(this, Cast<APABaseCharacter>(GetPawn()));
	}
}

void APABasePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (IsLocalController())
	{
		// Hỗ trợ trực tiếp điều khiển WASD trên bàn phím (độc lập với EnhancedInput asset)
		FVector2D DirectMoveVector = FVector2D::ZeroVector;
		if (IsInputKeyDown(EKeys::W)) { DirectMoveVector.Y += 1.0f; }
		if (IsInputKeyDown(EKeys::S)) { DirectMoveVector.Y -= 1.0f; }
		if (IsInputKeyDown(EKeys::D)) { DirectMoveVector.X += 1.0f; }
		if (IsInputKeyDown(EKeys::A)) { DirectMoveVector.X -= 1.0f; }

		if (APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn()))
		{
			if (!DirectMoveVector.IsNearlyZero())
			{
				DirectMoveVector.Normalize();
				ControlledCharacter->Input_MoveScreenRelative(DirectMoveVector);
			}
			else if (!MoveAction)
			{
				ControlledCharacter->Input_MoveScreenRelative(FVector2D::ZeroVector);
			}
		}

		// 1. Chỉ thực thi chiếu tia chuột nếu người chơi không đang ngắm bằng cần xoay analog gamepad
		if (!bIsActivelyUsingGamepadAim)
		{
			UpdateMouseAiming();
		}

		// 2. Cập nhật độ trôi đón đầu của Camera theo hướng ngắm (AC-2)
		UpdateCameraLookAhead(DeltaTime);

		// 3. Quét tia và làm mờ dither vật thể che khuất tầm nhìn (AC-3)
		UpdateCameraOcclusion(DeltaTime);

		// 4. Cập nhật trạng thái tương tác HUD (phím F) và tự động đóng khi rời xa > 500cm (DECISIONS §13)
		if (ActiveGameplayHUDWidget)
		{
			if (ActiveGameplayHUDWidget->IsAnyInteractiveWindowOpen())
			{
				bool bShouldClose = false;
				if (!CurrentInteractingActor.IsValid())
				{
					bShouldClose = true;
				}
				else if (APawn* ControlledPawn = GetPawn())
				{
					const float Dist = FVector::Dist(ControlledPawn->GetActorLocation(), CurrentInteractingActor->GetActorLocation());
					if (Dist > 500.0f || PAServerRequestValidation::IsActorInCombat(ControlledPawn))
					{
						bShouldClose = true;
					}
				}
				if (bShouldClose)
				{
					HandleCloseInteractiveWindows();
				}
				ActiveGameplayHUDWidget->SetInteractionPrompt(TEXT(""), false);
			}
			else
			{
				AActor* NearbyActor = FindBestInteractableActor(300.0f);
				if (NearbyActor)
				{
					FString PromptText = TEXT("[F] Tương Tác");
					if (NearbyActor->FindComponentByClass<UPAMerchantComponent>())
					{
						PromptText = TEXT("[F] Giao Dịch (Thương Nhân)");
					}
					else if (NearbyActor->FindComponentByClass<UPABlacksmithComponent>())
					{
						PromptText = TEXT("[F] Rèn / Sửa Đồ (Lò Rèn)");
					}
					ActiveGameplayHUDWidget->SetInteractionPrompt(PromptText, true);
				}
				else
				{
					ActiveGameplayHUDWidget->SetInteractionPrompt(TEXT(""), false);
				}
			}
		}
	}
}

void APABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		// Chuột trái chém đòn cận chiến trực tiếp
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &APABasePlayerController::HandleDirectAttack);

		// Phím Spacebar lướt né I-frame
		InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &APABasePlayerController::HandleDirectDash);

		// Phím E hoặc chuột phải thi triển kết liễu Finisher
		InputComponent->BindKey(EKeys::E, IE_Pressed, this, &APABasePlayerController::HandleDirectFinisher);
		InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &APABasePlayerController::HandleDirectFinisher);

		// Phím F tương tác ngữ cảnh (Merchant, Forge)
		InputComponent->BindKey(EKeys::F, IE_Pressed, this, &APABasePlayerController::HandleInteract);

		// Phím Escape đóng các cửa sổ tương tác
		InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &APABasePlayerController::HandleCloseInteractiveWindows);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Liên kết Action di chuyển (WASD / Left Stick)
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APABasePlayerController::HandleMoveTriggered);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APABasePlayerController::HandleMoveCompleted);
		}

		// Liên kết Action ngắm bắn Gamepad (Right Stick)
		if (GamepadAimAction)
		{
			EnhancedInputComponent->BindAction(GamepadAimAction, ETriggerEvent::Triggered, this, &APABasePlayerController::HandleGamepadAimTriggered);
		}
	}
}

void APABasePlayerController::HandleDirectAttack()
{
	if (APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn()))
	{
		if (UAbilitySystemComponent* ASC = ControlledCharacter->GetAbilitySystemComponent())
		{
			ASC->TryActivateAbilityByClass(UPAGameplayAbility_MeleeAttack::StaticClass());
		}
	}
}

void APABasePlayerController::HandleDirectDash()
{
	if (APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn()))
	{
		if (UAbilitySystemComponent* ASC = ControlledCharacter->GetAbilitySystemComponent())
		{
			ASC->TryActivateAbilityByClass(UPAGameplayAbility_Dash::StaticClass());
		}
	}
}

void APABasePlayerController::HandleDirectFinisher()
{
	if (APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn()))
	{
		if (UAbilitySystemComponent* ASC = ControlledCharacter->GetAbilitySystemComponent())
		{
			ASC->TryActivateAbilityByClass(UPAGameplayAbility_Finisher::StaticClass());
		}
	}
}

void APABasePlayerController::HandleMoveTriggered(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn()))
	{
		ControlledCharacter->Input_MoveScreenRelative(MovementVector);
	}
}

void APABasePlayerController::HandleMoveCompleted(const FInputActionValue& Value)
{
	if (APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn()))
	{
		ControlledCharacter->Input_MoveScreenRelative(FVector2D::ZeroVector);
	}
}

void APABasePlayerController::HandleGamepadAimTriggered(const FInputActionValue& Value)
{
	const FVector2D RawStickVector = Value.Get<FVector2D>();

	// Lọc input qua vùng chết Gamepad Deadzone (0.20)
	const FVector2D FilteredAimVector = UPAIsometricMovementMath::FilterGamepadAimInput(RawStickVector, GamepadAimDeadzone);

	if (!FilteredAimVector.IsNearlyZero())
	{
		bIsActivelyUsingGamepadAim = true;

		// Chuyển đổi input 2D của cần analog sang hướng thế giới 3D dựa trên góc Yaw của Camera (45 độ)
		const FVector AimWorldDirection = UPAIsometricMovementMath::CalculateWorldDirection(FilteredAimVector, 45.0f);

		if (APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn()))
		{
			ControlledCharacter->UpdateAimDirection(AimWorldDirection);
		}
	}
	else
	{
		// Nếu thả cần analog về giữa, cho phép chuyển lại quyền điều khiển cho chuột
		bIsActivelyUsingGamepadAim = false;
	}
}

void APABasePlayerController::UpdateMouseAiming()
{
	APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn());
	if (!ControlledCharacter)
	{
		return;
	}

	FVector RayOrigin, RayDirection;
	// Chiếu tọa độ pixel con trỏ chuột sang Ray Origin và Ray Direction trong thế giới 3D
	if (DeprojectMousePositionToWorld(RayOrigin, RayDirection))
	{
		const float CharacterFootZ = ControlledCharacter->GetActorLocation().Z;
		FVector GroundIntersection;

		// Tìm giao điểm của tia với mặt phẳng ngang Z của nhân vật
		if (UPAIsometricMovementMath::DeprojectRayToGroundPlane(RayOrigin, RayDirection, CharacterFootZ, GroundIntersection))
		{
			LastGroundHitLocation = GroundIntersection;
			ControlledCharacter->UpdateAimFromTargetLocation(GroundIntersection);
		}
	}
}

void APABasePlayerController::UpdateCameraLookAhead(float DeltaTime)
{
	APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn());
	if (!ControlledCharacter)
	{
		return;
	}

	FVector TargetOffset = FVector::ZeroVector;

	if (bEnableLookAhead)
	{
		const FVector CharacterLocation = ControlledCharacter->GetActorLocation();
		TargetOffset = UPAIsometricMovementMath::CalculateLookAheadTargetOffset(
			CharacterLocation,
			LastGroundHitLocation,
			MaxLookAheadDistance,
			UPAIsometricMovementMath::kDefaultLookAheadDistanceScale);
	}

	CurrentLookAheadOffset = UPAIsometricMovementMath::InterpolateLookAheadOffset(
		CurrentLookAheadOffset,
		TargetOffset,
		DeltaTime,
		LookAheadReturnSpeed);

	ControlledCharacter->SetCameraSocketOffset(CurrentLookAheadOffset);
}

void APABasePlayerController::UpdateCameraOcclusion(float DeltaTime)
{
	APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn());
	if (!ControlledCharacter || !ControlledCharacter->GetFollowCamera())
	{
		ResetOccludedComponents();
		return;
	}

	const FVector CameraLocation = ControlledCharacter->GetFollowCamera()->GetComponentLocation();
	const FVector CharacterLocation = ControlledCharacter->GetActorLocation();

	// Quét Sphere Sweep (bán kính 15 cm) từ camera đến vị trí nhân vật
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams(TEXT("CameraOcclusionTrace"), false, ControlledCharacter);
	QueryParams.AddIgnoredActor(ControlledCharacter);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FCollisionShape SweepSphere = FCollisionShape::MakeSphere(15.0f);
	World->SweepMultiByChannel(
		HitResults,
		CameraLocation,
		CharacterLocation,
		FQuat::Identity,
		ECC_Visibility,
		SweepSphere,
		QueryParams);

	TArray<TWeakObjectPtr<UPrimitiveComponent>> NewOccludedList;

	for (const FHitResult& Hit : HitResults)
	{
		if (UPrimitiveComponent* HitComp = Hit.GetComponent())
		{
			// Kiểm tra vật cản nằm trong bán kính hình trụ OcclusionFadeRadius (200 cm)
			if (UPAIsometricMovementMath::IsPointWithinOcclusionCylinder(
					Hit.ImpactPoint,
					CameraLocation,
					CharacterLocation,
					OcclusionFadeRadius))
			{
				NewOccludedList.Add(HitComp);
				if (UMeshComponent* MeshComp = Cast<UMeshComponent>(HitComp))
				{
					MeshComp->SetScalarParameterValueOnMaterials(DitherMaterialParameterName, OcclusionDitherOpacity);
				}
			}
		}
	}

	// Khôi phục vật liệu cho các component không còn bị che khuất
	for (const TWeakObjectPtr<UPrimitiveComponent>& OldCompPtr : CurrentlyOccludedComponents)
	{
		if (OldCompPtr.IsValid() && !NewOccludedList.Contains(OldCompPtr))
		{
			if (UMeshComponent* MeshComp = Cast<UMeshComponent>(OldCompPtr.Get()))
			{
				MeshComp->SetScalarParameterValueOnMaterials(DitherMaterialParameterName, 1.0f);
			}
		}
	}

	CurrentlyOccludedComponents = MoveTemp(NewOccludedList);
}

void APABasePlayerController::ResetOccludedComponents()
{
	for (const TWeakObjectPtr<UPrimitiveComponent>& CompPtr : CurrentlyOccludedComponents)
	{
		if (CompPtr.IsValid())
		{
			if (UMeshComponent* MeshComp = Cast<UMeshComponent>(CompPtr.Get()))
			{
				MeshComp->SetScalarParameterValueOnMaterials(DitherMaterialParameterName, 1.0f);
			}
		}
	}
	CurrentlyOccludedComponents.Empty();
}

void APABasePlayerController::HandleInteract()
{
	if (!IsLocalController())
	{
		return;
	}

	if (ActiveGameplayHUDWidget && ActiveGameplayHUDWidget->IsAnyInteractiveWindowOpen())
	{
		HandleCloseInteractiveWindows();
		return;
	}

	AActor* BestInteractable = FindBestInteractableActor(300.0f);
	if (!BestInteractable)
	{
		return;
	}

	APABaseCharacter* ControlledCharacter = Cast<APABaseCharacter>(GetPawn());
	if (!ControlledCharacter)
	{
		return;
	}

	// 1. Kiểm tra Thương Nhân Lang Thang / NPC Merchant
	if (UPAMerchantComponent* Merchant = BestInteractable->FindComponentByClass<UPAMerchantComponent>())
	{
		EPATransactionError Error = EPATransactionError::None;
		const bool bInCombat = ControlledCharacter->GetAbilitySystemComponent() &&
			ControlledCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.InCombat")));

		if (Merchant->ValidateInteraction(ControlledCharacter, bInCombat, Error))
		{
			CurrentInteractingActor = BestInteractable;
			if (ActiveGameplayHUDWidget)
			{
				const int32 PlayerKarma = FMath::RoundToInt(ControlledCharacter->Karma);
				ActiveGameplayHUDWidget->OpenShop(
					Merchant,
					ControlledCharacter->GetInventoryComponent(),
					ControlledCharacter->GetCurrencyComponent(),
					PlayerKarma,
					ServiceRequestComponent
				);
			}
			return;
		}
	}

	// 2. Kiểm tra Lò Rèn / Đe Rèn Blacksmith Forge
	if (UPABlacksmithComponent* Forge = BestInteractable->FindComponentByClass<UPABlacksmithComponent>())
	{
		EPACraftingError Error = EPACraftingError::None;
		const bool bInCombat = ControlledCharacter->GetAbilitySystemComponent() &&
			ControlledCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.InCombat")));

		if (Forge->ValidateInteraction(ControlledCharacter, bInCombat, Error))
		{
			CurrentInteractingActor = BestInteractable;
			if (ActiveGameplayHUDWidget)
			{
				ActiveGameplayHUDWidget->OpenForge(
					Forge,
					ControlledCharacter->GetInventoryComponent(),
					ControlledCharacter->GetCurrencyComponent(),
					ServiceRequestComponent
				);
			}
			return;
		}
	}
}

void APABasePlayerController::HandleCloseInteractiveWindows()
{
	if (ActiveGameplayHUDWidget)
	{
		ActiveGameplayHUDWidget->CloseInteractiveWindows();
	}
	CurrentInteractingActor.Reset();
}

AActor* APABasePlayerController::FindBestInteractableActor(float MaxDistance) const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !GetWorld())
	{
		return nullptr;
	}

	const FVector PlayerLoc = ControlledPawn->GetActorLocation();
	AActor* BestActor = nullptr;
	float ClosestDistSq = FMath::Square(MaxDistance);

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Candidate = *It;
		if (!Candidate || Candidate == ControlledPawn)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(PlayerLoc, Candidate->GetActorLocation());
		if (DistSq > ClosestDistSq)
		{
			continue;
		}

		const bool bHasMerchant = Candidate->FindComponentByClass<UPAMerchantComponent>() != nullptr;
		const bool bHasForge = Candidate->FindComponentByClass<UPABlacksmithComponent>() != nullptr;

		if (bHasMerchant || bHasForge)
		{
			ClosestDistSq = DistSq;
			BestActor = Candidate;
		}
	}

	return BestActor;
}


