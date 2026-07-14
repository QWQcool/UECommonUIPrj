// Fill out your copyright notice in the Description page of Project Settings.



#include "Widgets/Options/Widget_OptionsScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Components/FrontendTabListWidgetBase.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
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

	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this,&ThisClass::OnOptionsTabSelected);
	
	// CommonListView_OptionsList->OnItemIsHoveredChanged().AddUObject(this,&ThisClass::OnListViewItemHovered);
	// CommonListView_OptionsList->OnItemSelectionChanged().AddUObject(this,&ThisClass::OnListViewItemSelected);
}

void UWidget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	for (UListDataObject_Collection* TabCollection : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollection)
		{
			continue;
		}

		const FName TabID = TabCollection->GetDataID();

		if (TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabID) != nullptr)
		{
			continue;
		}

		TabListWidget_OptionsTabs->RequestRegisterTab(TabID,TabCollection->GetDataDisplayName());
	}
}

void UWidget_OptionsScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

UOptionsDataRegistry* UWidget_OptionsScreen::GetOrCreateDataRegistry()
{	
	if (!CreatedOwningDataRegistry)
	{
		CreatedOwningDataRegistry = NewObject<UOptionsDataRegistry>();
		CreatedOwningDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
	}

	checkf(CreatedOwningDataRegistry,TEXT("Data registry for options screen is not valid"));

	return CreatedOwningDataRegistry;
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

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabId)
{
	// DetailsView_ListEntryInfo->ClearDetailsViewInfo();

	Debug::Print(TEXT("OnOptionsTabSelected: %s") + TabId.ToString());
	
	// TArray<UListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabId);

	// CommonListView_OptionsList->SetListItems(FoundListSourceItems);
	// CommonListView_OptionsList->RequestRefresh();
	//
	// if (CommonListView_OptionsList->GetNumItems() != 0)
	// {
	// 	CommonListView_OptionsList->NavigateToIndex(0);
	// 	CommonListView_OptionsList->SetSelectedIndex(0);
	// }
	//
	// ResettableDataArray.Empty();

	// for (UListDataObject_Base* FoundListSourceItem : FoundListSourceItems)
	// {
	// 	if (!FoundListSourceItem)
	// 	{
	// 		continue;
	// 	}
	//
	// 	if (!FoundListSourceItem->OnListDataModified.IsBoundToObject(this))
	// 	{
	// 		FoundListSourceItem->OnListDataModified.AddUObject(this,&ThisClass::OnListViewListDataModified);
	// 	}
	//
	// 	if (FoundListSourceItem->CanResetBackToDefaultValue())
	// 	{
	// 		ResettableDataArray.AddUnique(FoundListSourceItem);
	// 	}
	// }

	// if (ResettableDataArray.IsEmpty())
	// {
	// 	RemoveActionBinding(ResetActionHandle);
	// }
	// else
	// {
	// 	if (!GetActionBindings().Contains(ResetActionHandle))
	// 	{
	// 		AddActionBinding(ResetActionHandle);
	// 	}
	// }
}
