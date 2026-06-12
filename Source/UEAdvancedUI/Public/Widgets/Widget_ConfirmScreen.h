// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Widget_ConfirmScreen.generated.h"

class UCommonTextBlock;
class UDynamicEntryBox;

//这个结构体用于描述确认框按钮的信息
USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EConfirmScreenButtonType ConfirmScreenButtonType = EConfirmScreenButtonType::Unknown;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FText ButtonTextToDisplay;
};

//这个类的作用是用于创建那种类型的确认框的信息
UCLASS()
class UEADVANCEDUI_API UConfirmScreenInfoObject : public UObject
{
	GENERATED_BODY()

public:
	static UConfirmScreenInfoObject* CreateOKScreen(const FText& InScreenTitle,const FText& InScreenMsg);
	static UConfirmScreenInfoObject* CreateYesNoScreen(const FText& InScreenTitle,const FText& InScreenMsg);
	static UConfirmScreenInfoObject* CreateOkCancelScreen(const FText& InScreenTitle,const FText& InScreenMsg);

	UPROPERTY(Transient)
	FText ScreenTitle;
	
	UPROPERTY(Transient)
	FText ScreenMessage;

	//根据这个数组的数量，我们可以知道这个确认框有几个按钮
	UPROPERTY(Transient)
	TArray<FConfirmScreenButtonInfo> AvailableScreenButtons;
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UEADVANCEDUI_API UWidget_ConfirmScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
public:
	//Gets called outside of the class when this widget is contructed and before it's pushed to the modal stack
	void InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObject,TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback);

protected:
	//~ Begin UCommonActivatableWidget Interface
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	//~ End UCommonActivatableWidget Interface

private:
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_Title;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_Message;

	UPROPERTY(meta = (BindWidget))
	UDynamicEntryBox* DynamicEntryBox_Buttons;
};
