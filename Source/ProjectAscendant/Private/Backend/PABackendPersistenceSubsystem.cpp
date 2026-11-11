// Copyright Project Ascendant. All Rights Reserved.

#include "Backend/PABackendPersistenceSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"

void UPABackendPersistenceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Đọc cấu hình từ command-line nếu có (-backendurl=http://... -serversecret=...)
	FString CustomUrl;
	if (FParse::Value(FCommandLine::Get(), TEXT("backendurl="), CustomUrl))
	{
		BackendBaseUrl = CustomUrl.TrimStartAndEnd();
	}

	FString CustomSecret;
	if (FParse::Value(FCommandLine::Get(), TEXT("serversecret="), CustomSecret))
	{
		ServerSecret = CustomSecret.TrimStartAndEnd();
	}

	UE_LOG(LogTemp, Log, TEXT("[PABackendPersistenceSubsystem] Initialized with BaseUrl: %s"), *BackendBaseUrl);
}

void UPABackendPersistenceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPABackendPersistenceSubsystem::ValidateClientToken(
	const FString& Token,
	const FString& ExpectedAccountId,
	FPABackendTokenValidationDelegate Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/v1/server/validate-token"), *BackendBaseUrl));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("X-Server-Secret"), ServerSecret);

	TSharedPtr<FJsonObject> JsonObj = MakeShared<FJsonObject>();
	JsonObj->SetStringField(TEXT("token"), Token);
	if (!ExpectedAccountId.IsEmpty())
	{
		JsonObj->SetStringField(TEXT("expected_account_id"), ExpectedAccountId);
	}

	FString RequestPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestPayload);
	FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(RequestPayload);

	HttpRequest->OnProcessRequestComplete().BindLambda(
		[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (!bConnectedSuccessfully || !Response.IsValid())
			{
				Callback.ExecuteIfBound(false, TEXT("Connection failed"));
				return;
			}

			if (Response->GetResponseCode() == 200)
			{
				TSharedPtr<FJsonObject> ResObj;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				if (FJsonSerializer::Deserialize(Reader, ResObj) && ResObj.IsValid())
				{
					const bool bValid = ResObj->GetBoolField(TEXT("valid"));
					const FString AccountId = ResObj->GetStringField(TEXT("account_id"));
					Callback.ExecuteIfBound(bValid, AccountId);
					return;
				}
			}

			Callback.ExecuteIfBound(false, TEXT(""));
		});

	HttpRequest->ProcessRequest();
}

void UPABackendPersistenceSubsystem::LoadCharacter(
	const FString& CharacterId,
	FPABackendLoadCharacterDelegate Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/v1/server/characters/%s"), *BackendBaseUrl, *CharacterId));
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("X-Server-Secret"), ServerSecret);

	HttpRequest->OnProcessRequestComplete().BindLambda(
		[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			FPABackendCharacterData LoadedData;
			if (!bConnectedSuccessfully || !Response.IsValid() || Response->GetResponseCode() != 200)
			{
				Callback.ExecuteIfBound(false, LoadedData);
				return;
			}

			TSharedPtr<FJsonObject> ResObj;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResObj) && ResObj.IsValid())
			{
				LoadedData.CharacterId = ResObj->GetStringField(TEXT("character_id"));
				LoadedData.AccountId = ResObj->GetStringField(TEXT("account_id"));
				LoadedData.CharacterName = ResObj->GetStringField(TEXT("character_name"));
				LoadedData.CharacterLevel = ResObj->GetIntegerField(TEXT("character_level"));
				LoadedData.CharacterExp = ResObj->GetIntegerField(TEXT("character_exp"));
				LoadedData.PrimaryClassTag = ResObj->GetStringField(TEXT("primary_class_tag"));
				if (ResObj->HasField(TEXT("secondary_class_tag")) && !ResObj->HasTypedField<EJson::Null>(TEXT("secondary_class_tag")))
				{
					LoadedData.SecondaryClassTag = ResObj->GetStringField(TEXT("secondary_class_tag"));
				}

				const TArray<TSharedPtr<FJsonValue>>* QuestsArray = nullptr;
				if (ResObj->TryGetArrayField(TEXT("completed_quests"), QuestsArray))
				{
					for (const auto& QVal : *QuestsArray)
					{
						LoadedData.CompletedQuests.Add(QVal->AsString());
					}
				}

				Callback.ExecuteIfBound(true, LoadedData);
				return;
			}

			Callback.ExecuteIfBound(false, LoadedData);
		});

	HttpRequest->ProcessRequest();
}

void UPABackendPersistenceSubsystem::SaveCharacter(
	const FPABackendCharacterData& CharacterData,
	FPABackendSaveCharacterDelegate Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/v1/server/characters/%s/save"), *BackendBaseUrl, *CharacterData.CharacterId));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("X-Server-Secret"), ServerSecret);

	TSharedPtr<FJsonObject> JsonObj = MakeShared<FJsonObject>();
	JsonObj->SetNumberField(TEXT("character_level"), CharacterData.CharacterLevel);
	JsonObj->SetNumberField(TEXT("character_exp"), CharacterData.CharacterExp);
	JsonObj->SetStringField(TEXT("primary_class_tag"), CharacterData.PrimaryClassTag);
	if (!CharacterData.SecondaryClassTag.IsEmpty())
	{
		JsonObj->SetStringField(TEXT("secondary_class_tag"), CharacterData.SecondaryClassTag);
	}

	TArray<TSharedPtr<FJsonValue>> QuestsJson;
	for (const FString& Quest : CharacterData.CompletedQuests)
	{
		QuestsJson.Add(MakeShared<FJsonValueString>(Quest));
	}
	JsonObj->SetArrayField(TEXT("completed_quests"), QuestsJson);

	TArray<TSharedPtr<FJsonValue>> ItemsJson;
	for (const FPASavedItemInstance& It : CharacterData.Items)
	{
		TSharedPtr<FJsonObject> ItObj = MakeShared<FJsonObject>();
		ItObj->SetStringField(TEXT("item_instance_id"), It.ItemInstanceUID.ToString(EGuidFormats::DigitsWithHyphensLower));
		ItObj->SetStringField(TEXT("item_def_id"), It.ItemDefId.ToString());
		ItObj->SetNumberField(TEXT("slot_index"), It.SlotIndex);
		ItObj->SetNumberField(TEXT("quantity"), It.StackCount);
		ItObj->SetNumberField(TEXT("durability"), It.CurrentDurability);
		ItObj->SetNumberField(TEXT("enhancement_level"), It.EnhancementLevel);
		ItemsJson.Add(MakeShared<FJsonValueObject>(ItObj));
	}
	JsonObj->SetArrayField(TEXT("items"), ItemsJson);

	FString RequestPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestPayload);
	FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(RequestPayload);

	HttpRequest->OnProcessRequestComplete().BindLambda(
		[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully && Response.IsValid() && Response->GetResponseCode() == 200)
			{
				Callback.ExecuteIfBound(true, TEXT("Saved successfully"));
			}
			else
			{
				const FString Err = Response.IsValid() ? Response->GetContentAsString() : TEXT("Connection failed");
				Callback.ExecuteIfBound(false, Err);
			}
		});

	HttpRequest->ProcessRequest();
}

void UPABackendPersistenceSubsystem::SwapInventorySlots(
	const FString& OwnerId,
	const FString& SlotType,
	int32 SlotA,
	int32 SlotB,
	FPABackendSaveCharacterDelegate Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/v1/server/inventory/swap-slots"), *BackendBaseUrl));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("X-Server-Secret"), ServerSecret);

	TSharedPtr<FJsonObject> JsonObj = MakeShared<FJsonObject>();
	JsonObj->SetStringField(TEXT("owner_type"), TEXT("CHARACTER"));
	JsonObj->SetStringField(TEXT("owner_id"), OwnerId);
	JsonObj->SetStringField(TEXT("slot_type"), SlotType);
	JsonObj->SetNumberField(TEXT("slot_a"), SlotA);
	JsonObj->SetNumberField(TEXT("slot_b"), SlotB);

	FString RequestPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestPayload);
	FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(RequestPayload);

	HttpRequest->OnProcessRequestComplete().BindLambda(
		[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully && Response.IsValid() && Response->GetResponseCode() == 200)
			{
				Callback.ExecuteIfBound(true, TEXT("Slots swapped successfully"));
			}
			else
			{
				const FString Err = Response.IsValid() ? Response->GetContentAsString() : TEXT("Connection failed");
				Callback.ExecuteIfBound(false, Err);
			}
		});

	HttpRequest->ProcessRequest();
}

void UPABackendPersistenceSubsystem::PromoteClassWithScroll(
	const FString& CharacterId,
	const FGuid& ScrollInstanceId,
	const FString& TargetClassTag,
	bool bIsSecondary,
	FPABackendPromotionDelegate Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/v1/server/characters/%s/promote-class"), *BackendBaseUrl, *CharacterId));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("X-Server-Secret"), ServerSecret);

	TSharedPtr<FJsonObject> JsonObj = MakeShared<FJsonObject>();
	JsonObj->SetStringField(TEXT("scroll_item_instance_id"), ScrollInstanceId.ToString(EGuidFormats::DigitsWithHyphensLower));
	JsonObj->SetStringField(TEXT("target_class_tag"), TargetClassTag);
	JsonObj->SetBoolField(TEXT("is_secondary"), bIsSecondary);

	FString RequestPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestPayload);
	FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
	HttpRequest->SetContentAsString(RequestPayload);

	HttpRequest->OnProcessRequestComplete().BindLambda(
		[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully && Response.IsValid() && Response->GetResponseCode() == 200)
			{
				Callback.ExecuteIfBound(true, TEXT("Promoted successfully"));
			}
			else
			{
				const FString Err = Response.IsValid() ? Response->GetContentAsString() : TEXT("Promotion failed");
				Callback.ExecuteIfBound(false, Err);
			}
		});

	HttpRequest->ProcessRequest();
}
