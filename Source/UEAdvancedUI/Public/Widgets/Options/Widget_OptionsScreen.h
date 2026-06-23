// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Widget_OptionsScreen.generated.h"

class UOptionsDataRegistry;
class UFrontendTabListWidgetBase;
class UFrontendCommonListView;
class UWidget_OptionsDetailsView;
class UListDataObject_Base;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UEADVANCEDUI_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UCommonActivatableWidget Interface
	// virtual void NativeOnActivated() override;
	// virtual void NativeOnDeactivated() override; 
	// virtual UWidget* NativeGetDesiredFocusTarget() const override;
	//~ End UCommonActivatableWidget Interface
	
private:

	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();
	
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;

	FUIActionBindingHandle ResetActionHandle;

	// UPROPERTY(Transient)
	// TArray<UListDataObject_Base*> ResettableDataArray;
};
