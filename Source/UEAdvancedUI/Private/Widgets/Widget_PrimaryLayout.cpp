// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Widget_PrimaryLayout.h"
#include "FrontendDebugHelper.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
	if (!RegisteredWidgetStackMap.Contains(InTag))
	{
		ensureMsgf(false, TEXT("Widget stack with tag %s not found"), *InTag.ToString());
		return nullptr;
	}
	
	return RegisteredWidgetStackMap.FindRef(InTag);
}

void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack"))FGameplayTag StackTag, UCommonActivatableWidgetContainerBase* Container)
{
	if (!IsDesignTime())
	{
		if (!RegisteredWidgetStackMap.Contains(StackTag))
		{
			RegisteredWidgetStackMap.Add(StackTag, Container);

			// Debug::Print(TEXT("Widget Stack Registered under the tag: ") + StackTag.ToString());
		}
	}
}
