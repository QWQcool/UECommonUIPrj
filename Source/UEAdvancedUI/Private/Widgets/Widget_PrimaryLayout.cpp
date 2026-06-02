// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Widget_PrimaryLayout.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
	checkf(RegisteredWidgetStackMap.Contains(InTag), TEXT("Widget stack with tag %s not found"), *InTag.ToString());
	
	return RegisteredWidgetStackMap.FindRef(InTag);
}

void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack"))FGameplayTag StackTag, UCommonActivatableWidgetContainerBase* Container)
{
	if (!IsDesignTime())
	{
		if (!RegisteredWidgetStackMap.Contains(StackTag))
		{
			RegisteredWidgetStackMap.Add(StackTag, Container);
		}
	}
}
