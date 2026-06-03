// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/FrontendUISubsystem.h"
#include "Engine/AssetManager.h"
#include "Widgets/Widget_ActivatableBase.h"

#include "FrontendDebugHelper.h"
#include "Widgets/Widget_PrimaryLayout.h"

// CommonUI 模块头文件
#include "Widgets/CommonActivatableWidgetContainer.h"

UFrontendUISubsystem* UFrontendUISubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		return UGameInstance::GetSubsystem<UFrontendUISubsystem>(World->GetGameInstance());
	}
	return nullptr;
}

bool UFrontendUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 专用服务器不需要前端UI，不创建Subsystem
	UGameInstance* GameInstance = Cast<UGameInstance>(Outer);
	if (!GameInstance)
	{
		ensureMsgf(false, TEXT("Outer is not a UGameInstance"));
		return false;
	}
	
	if (GameInstance->IsDedicatedServerInstance())
	{
		return false;
	}
	
	// 客户端和单机模式需要创建Subsystem
	// 如果是基类，直接创建；如果有派生类，由派生类决定
	TArray<UClass*> FoundClasses;
	GetDerivedClasses(GetClass(), FoundClasses);
	
	return FoundClasses.IsEmpty();
}

void UFrontendUISubsystem::RegisterCreatedPrimaryLayout(UWidget_PrimaryLayout* InCreateWidget)
{
	if (!InCreateWidget)
	{
		ensureMsgf(false, TEXT("RegisterCreatedPrimaryLayout: InCreateWidget is null"));
		return;
	}
	
	CreatedPrimaryLayout = InCreateWidget;

	Debug::Print(TEXT("Primary Layout Registered"));
}

void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag,
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallback)
{
	if (InSoftWidgetClass.IsNull())
	{
		ensureMsgf(false, TEXT("PushSoftWidgetToStackAsync: InSoftWidgetClass is null"));
		return;
	}

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[InSoftWidgetClass, this, InWidgetStackTag, AsyncPushStateCallback]()
		{
			UClass* LoadWidgetClass = InSoftWidgetClass.Get();
			if (LoadWidgetClass && CreatedPrimaryLayout)
			{
				// TODO: Create and push widget
				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
				UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
					LoadWidgetClass,
					[AsyncPushStateCallback](UWidget_ActivatableBase& CreateWidgetInstance)
					{ 
						AsyncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreateWidgetInstance);
					}
					);
				AsyncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);
			}
		})
	);
}
