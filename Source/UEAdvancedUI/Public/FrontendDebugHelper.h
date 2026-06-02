// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"

/**
 * FrontendDebugHelper
 * 前端调试辅助工具命名空间，提供常用的调试功能和宏定义
 */
namespace Debug
{
	/** 输出调试信息到屏幕和日志 */
	inline void Print(const FString& Message, int InKey = -1, const FColor& InColor = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, 7.f, InColor, Message);
		}
		UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	}

	/** 仅输出到日志（不带屏幕显示） */
	inline void Log(const FString& Message)
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	}
}