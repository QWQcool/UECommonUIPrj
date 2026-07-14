// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"

#include "FrontendSettings/FrontendGameUserSettings.h"
#include "FrontendFunctionLibrary.h"
#include "FrontendGameplayTags.h"

#include "Internationalization/StringTableRegistry.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"


#include "FrontendDebugHelper.h"

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings,SetterOrGetterFuncName))

#define GET_DESCRIPTION(InKey) LOCTABLE("/Game/UI/StringTables/ST_OptionsScreenDescription.ST_OptionsScreenDescription", InKey)

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab(InOwningLocalPlayer);
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* GameplayTabCollection = NewObject<UListDataObject_Collection>();
	GameplayTabCollection->SetDataID(FName("GameplayTabCollection"));
	GameplayTabCollection->SetDataDisplayName(FText::FromString(TEXT("Gameplay")));

	//This is the full code for constructing data interactor helper
	/*TSharedPtr<FOptionsDataInteractionHelper> ConstructedHelper = 
	MakeShared<FOptionsDataInteractionHelper>(
		GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings,GetCurrentGameDifficulty)
	);*/
	
	//Game Difficulty
	{
		
	}

	RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioTabCollection = NewObject<UListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName("AudioTabCollection"));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("Audio")));

	//This is the full code for constructing data interactor helper
	/*TSharedPtr<FOptionsDataInteractionHelper> ConstructedHelper = 
	MakeShared<FOptionsDataInteractionHelper>(
		GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings,GetCurrentGameDifficulty)
	);*/
	
	//Game Difficulty
	{
		
	}

	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName("VideoTabCollection"));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("Video")));

	//This is the full code for constructing data interactor helper
	/*TSharedPtr<FOptionsDataInteractionHelper> ConstructedHelper = 
	MakeShared<FOptionsDataInteractionHelper>(
		GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings,GetCurrentGameDifficulty)
	);*/
	
	//Game Difficulty
	{
		
	}

	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* ControlTabCollection = NewObject<UListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName("ControlTabCollection"));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("Control")));

	// TODO: Add control options items here

	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}