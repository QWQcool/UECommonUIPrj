// Fill out your copyright notice in the Description page of Project Settings.



#include "Widgets/Options/Widget_OptionsScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
// #include "Widgets/Options/OptionsDataRegistry.h"
// #include "Widgets/Components/FrontendTabListWidgetBase.h"
// #include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
// #include "Widgets/Components/FrontendCommonListView.h"
// #include "FontendSettings/FrontendGameUserSettings.h"
// #include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"
// #include "Widgets/Options/Widget_OptionsDetailsView.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"

#include "FrontendDebugHelper.h"

void UWidget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetBoundActionTriggered)
			)
		);
	}
	
	RegisterUIActionBinding(
		FBindUIActionArgs(
			ICommonInputModule::GetSettings().GetDefaultBackAction(),
			true,
			FSimpleDelegate::CreateUObject(this,&ThisClass::OnBackBoundActionTriggered)
		)
	);

	// TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this,&ThisClass::OnOptionsTabSelected);
	
	// CommonListView_OptionsList->OnItemIsHoveredChanged().AddUObject(this,&ThisClass::OnListViewItemHovered);
	// CommonListView_OptionsList->OnItemSelectionChanged().AddUObject(this,&ThisClass::OnListViewItemSelected);
}

void UWidget_OptionsScreen::OnResetBoundActionTriggered()
{
	Debug::Print(TEXT("ResetBoundActionTriggered"));
	// if (ResettableDataArray.IsEmpty())
	// {
		// return;
	// }

	return;
}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	DeactivateWidget();
}
