// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/FrontendUISubsystem.h"
#include "Engine/AssetManager.h"
#include "Widgets/Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widgets/Widget_ConfirmScreen.h"
#include "FrontendGameplayTags.h"
#include "FrontendFunctionLibrary.h"

#include "FrontendDebugHelper.h"

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

void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,TFunction<void(EAsyncPushWidgetState,UWidget_ActivatableBase*)> AysncPushStateCallback)
{
	check(!InSoftWidgetClass.IsNull());

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[InSoftWidgetClass,this,InWidgetStackTag,AysncPushStateCallback]()
			{
				UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
				
				check(LoadedWidgetClass && CreatedPrimaryLayout);

				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);

				UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
					LoadedWidgetClass,
					[AysncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance)
					{
						AysncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush,&CreatedWidgetInstance);
					}
				);

				AysncPushStateCallback(EAsyncPushWidgetState::AfterPush,CreatedWidget);
			}
		)
	);
}

void UFrontendUISubsystem::PushConfirmScreenToModalStackAynsc(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMsg, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
{	
	UConfirmScreenInfoObject* CreatedInfoObject = nullptr;

	switch (InScreenType)
	{
	case EConfirmScreenType::Ok:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle,InScreenMsg);
		break;

	case EConfirmScreenType::YesNo:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle,InScreenMsg);
		break;

	case EConfirmScreenType::OKCancel:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateOkCancelScreen(InScreenTitle,InScreenMsg);
		break;

	case EConfirmScreenType::Unkown:
		break;
	default:
		break;
	}

	check(CreatedInfoObject);

	PushSoftWidgetToStackAsync(
		FrontendGameplayTags::Frontend_WidgetStack_Modal,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_ConfirmScreen),
		[CreatedInfoObject,ButtonClickedCallback](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
		{
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
				CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject,ButtonClickedCallback);
			}
		}
	);
}

