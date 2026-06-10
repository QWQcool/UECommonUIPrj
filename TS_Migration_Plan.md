# UECommonUIPrj — TypeScript 迁移计划

> **文档版本**: v1.0  
> **生成日期**: 2026-06-10  
> **项目**: UECommonUIPrj / UEAdvancedUI 模块

---

## 一、项目现状概览

### 1.1 模块结构

```
UEAdvancedUI/
├── Private/
│   ├── AsyncActions/
│   │   └── AsyncAction_PushSoftWidget.cpp
│   ├── Controllers/
│   │   └── FrontendPlayerController.cpp
│   ├── FrontendFunctionLibrary.cpp
│   ├── FrontendGameplayTags.cpp
│   ├── FrontendSettings/
│   │   ├── FrontendDeveloperSettings.cpp
│   │   ├── FrontendGameUserSettings.cpp
│   │   └── FrontendLoadingScreenSettings.cpp
│   ├── MyGameInstance.cpp
│   ├── Subsystems/
│   │   └── FrontendUISubsystem.cpp
│   └── Widgets/
│       ├── Components/
│       │   └── FrontendCommonButtonBase.cpp
│       ├── Widget_ActivatableBase.cpp
│       └── Widget_PrimaryLayout.cpp
└── Public/
    ├── AsyncActions/
    │   └── AsyncAction_PushSoftWidget.h
    ├── Controllers/
    │   └── FrontendPlayerController.h
    ├── FrontendDebugHelper.h
    ├── FrontendFunctionLibrary.h
    ├── FrontendGameplayTags.h
    ├── FrontendSettings/
    │   ├── FrontendDeveloperSettings.h
    │   ├── FrontendGameUserSettings.h
    │   └── FrontendLoadingScreenSettings.h
    ├── MyGameInstance.h
    ├── Subsystems/
    │   └── FrontendUISubsystem.h
    └── Widgets/
        ├── Components/
        │   └── FrontendCommonButtonBase.h
        ├── Widget_ActivatableBase.h
        └── Widget_PrimaryLayout.h
```

### 1.2 已集成 Puerts/JsEnv

项目已通过 `MyGameInstance` 集成了 Puerts 运行时：

```cpp
// MyGameInstance.h
UCLASS()
class UEADVANCEDUI_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:
    virtual void OnStart() override;
    virtual void Shutdown() override;
private:
    TSharedPtr<puerts::FJsEnv> GameScript;
};

// MyGameInstance.cpp
void UMyGameInstance::OnStart()
{
    Super::OnStart();
    GameScript = MakeShared<puerts::FJsEnv>();
    GameScript->Start("MainGame");
}
```

**关键结论**: 项目已具备 TS 运行时基础，入口脚本为 `MainGame`。

---

## 二、C++ 类逐项 TS 迁移可行性分析

### 2.1 `UFrontendCommonButtonBase` — 按钮 UI 组件

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `UCLASS()` / `GENERATED_BODY()` 宏 | ❌ 不可迁移 | UE 反射系统核心，必须在 C++ 中声明 |
| `BindWidgetOptional` 属性绑定 | ❌ 不可迁移 | UE 控件绑定机制，必须 C++ 声明 |
| `EditAnywhere` 可编辑属性 | ❌ 不可迁移 | `UPROPERTY` 宏必须在 C++ 中 |
| `NativePreConstruct()` 虚函数覆写 | ❌ 不可迁移 | UUserWidget 生命周期回调，必须 C++ |
| `NativeOnCurrentTextStyleChanged()` | ❌ 不可迁移 | UCommonButtonBase 虚函数，必须 C++ |
| `NativeOnHovered()` / `NativeOnUnhovered()` | ❌ 不可迁移 | 输入事件虚函数，必须 C++ |
| `SetButtonText()` 逻辑 | ✅ 可迁移 | 纯逻辑，TS 可调用 C++ 控件方法 |
| `GetButtonDisplayText()` 逻辑 | ✅ 可迁移 | 纯逻辑 |
| `SetButtionDisplayImage()` 逻辑 | ⚠️ 部分可迁移 | `FSlateBrush` 构造在 TS 中较繁琐 |

**结论**: C++ 骨架（类声明、宏、属性绑定、虚函数覆写）不可迁移；业务逻辑可部分迁移。

#### 推荐迁移方案

```
┌──────────────────────────────────────┐
│      UFrontendCommonButtonBase       │
│         (C++ 抽象基类)               │
│  ┌─ 必须在 C++ ──────┐ ┌─ 可迁移 ─┐│
│  │ • UCLASS 宏        │ │ • SetButtonText│
│  │ • GENERATED_BODY   │ │ • GetDisplayText│
│  │ • BindWidget 属性  │ │               │
│  │ • EditAnywhere 属性│ │               │
│  │ • Native 虚函数覆写│ │               │
│  └────────────────────┘ └──────────────┘│
│                 │                      │
│    BlueprintImplementableEvent         │
│                 │                      │
│       ┌─────────┴─────────┐           │
│       │                    │           │
│  ┌────▼──────┐     ┌──────▼──────┐   │
│  │ 蓝图子类    │     │ TS (Puerts) │   │
│  │ BP_Button  │     │ 脚本逻辑     │   │
│  │ - 控件布局  │     │ - 调用方法   │   │
│  │ - 事件覆写  │     │ - 属性设置   │   │
│  └───────────┘     └─────────────┘   │
└──────────────────────────────────────┘
```

---

### 2.2 `UWidget_ActivatableBase` — 可激活控件基类

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `UCLASS()` 宏 | ❌ 不可迁移 | 同上 |
| `GetOwningFrontendPlayerController()` | ✅ 可迁移 | 纯逻辑，缓存 `AFrontendPlayerController*` |
| `CachedOwningFrontendPC` 缓存 | ❌ 不可迁移 | `TWeakObjectPtr` 在 TS 中需特殊处理 |

**结论**: 类骨架保留 C++；`GetOwningFrontendPlayerController` 逻辑可由 TS 调用。

---

### 2.3 `UWidget_PrimaryLayout` — 主布局控件

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `UCLASS()` 宏 | ❌ 不可迁移 | 同上 |
| `FindWidgetStackByTag()` | ✅ 可迁移 | 纯查找逻辑 |
| `RegisterWidgetStack()` | ✅ 可迁移 | 纯注册逻辑 |
| `RegisteredWidgetStackMap` | ❌ 不可迁移 | `UPROPERTY(Transient)` 容器 |

**结论**: 骨架和属性保留 C++；查找和注册逻辑可由 TS 调用。

---

### 2.4 `UFrontendUISubsystem` — UI 子系统

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `Get()` 静态方法 | ✅ 可迁移 | 纯查找逻辑 |
| `ShouldCreateSubsystem()` | ❌ 不可迁移 | USubsystem 接口覆写 |
| `RegisterCreatedPrimaryLayout()` | ✅ 可迁移 | 纯注册逻辑 |
| `PushSoftWidgetToStackAsync()` | ⚠️ 部分可迁移 | 异步加载和 Lambda 回调在 TS 中处理较复杂 |
| `OnButtonDescriptionTextUpdated` 委托 | ❌ 不可迁移 | `UPROPERTY(BlueprintAssignable)` 声明 |

**结论**: 子系统骨架保留 C++；`Get()`、`RegisterCreatedPrimaryLayout()` 可由 TS 调用。异步推送逻辑建议保留 C++，因为涉及 `UAssetManager::Get().GetStreamableManager().RequestAsyncLoad()` 和复杂 Lambda。

---

### 2.5 `UAsyncAction_PushSoftWidget` — 异步推送控件动作

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `PushSoftWidget()` 静态方法 | ❌ 不可迁移 | `UBlueprintAsyncActionBase` 子类，必须在 C++ |
| `Activate()` | ❌ 不可迁移 | 异步动作生命周期 |
| `OnWidgetCreatedBeforePush` / `AfterPush` 委托 | ✅ 可由 TS 监听 | TS 可绑定委托回调 |
| Lambda 中的 `SetFocus` 逻辑 | ✅ 可迁移 | 纯逻辑 |

**结论**: 整个异步动作类必须在 C++，但回调逻辑可由 TS 处理。

---

### 2.6 `UFrontendFunctionLibrary` — 前端函数库

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `GetFrontendSoftWidgetClassByTag()` | ✅ 可完全迁移 | 纯查找映射表逻辑 |
| `GetOptionsSoftImageByTag()` | ✅ 可完全迁移 | 纯查找映射表逻辑 |

**结论**: **可完全迁移到 TS**。这两个函数是纯数据查询，从 `UFrontendDeveloperSettings` 的映射表中查找。TS 可通过 Puerts 直接调用 C++ 的 `GetDefault<UFrontendDeveloperSettings>()` 来实现相同功能。

---

### 2.7 `AFrontendPlayerController` — 前端玩家控制器

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `OnPossess()` 虚函数覆写 | ❌ 不可迁移 | APlayerController 接口 |
| 相机查找逻辑 | ✅ 可迁移 | `GetAllActorsOfClassWithTag` + `SetViewTarget` |

**结论**: 骨架保留 C++；`OnPossess` 中的相机逻辑可迁移到 TS（但需通过 C++ 接口暴露）。

---

### 2.8 `UFrontendDeveloperSettings` — 开发者设置

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `FrontendWidgetMap` | ❌ 不可迁移 | `UPROPERTY(Config, EditAnywhere)` 必须在 C++ |
| `OptionsScreenSoftImageMap` | ❌ 不可迁移 | 同上 |

**结论**: 配置映射表必须在 C++ 中声明（因为涉及 `Config` 和 `EditAnywhere`），但 TS 可通过 Puerts 读取这些配置。

---

### 2.9 `UFrontendGameUserSettings` — 游戏用户设置

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| 音量控制方法 | ✅ 可完全迁移 | 纯赋值逻辑 |
| 难度设置方法 | ✅ 可完全迁移 | 纯赋值逻辑 |
| 显示 Gamma 控制 | ✅ 可完全迁移 | 直接操作 `GEngine->DisplayGamma` |
| `UPROPERTY(Config)` 属性 | ❌ 不可迁移 | 必须在 C++ |

**结论**: Getter/Setter 逻辑可迁移到 TS；配置属性声明保留 C++。

---

### 2.10 `UFrontendLoadingScreenSettings` — 加载画面设置

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `GetLoadingScreenWidgetClassChecked()` | ✅ 可迁移 | 纯同步加载逻辑 |
| `SoftLoadingScreenWidgetClass` | ❌ 不可迁移 | `UPROPERTY(Config)` |
| `HoldLoadingScreenExtraSeconds` | ❌ 不可迁移 | `UPROPERTY(Config)` |

**结论**: 加载逻辑可迁移到 TS；配置属性保留 C++。

---

### 2.11 `UFrontendGameplayTags` — 游戏标签

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| 标签定义 | ❌ 不可迁移 | `UE_DEFINE_GAMEPLAY_TAG` 宏必须在 C++ |
| 标签使用 | ✅ 可迁移 | TS 可通过字符串引用标签 |

**结论**: 标签定义必须在 C++；TS 中通过字符串常量引用标签名称。

---

### 2.12 `UMyGameInstance` — 游戏实例

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `OnStart()` / `Shutdown()` | ❌ 不可迁移 | UGameInstance 生命周期 |
| `GameScript` 创建和启动 | ❌ 不可迁移 | Puerts 初始化 |
| TS 脚本调用逻辑 | ✅ 由 TS 侧实现 | 脚本入口后的逻辑 |

**结论**: Puerts 初始化必须在 C++；业务逻辑完全由 TS 实现。

---

### 2.13 `FrontendDebugHelper` — 调试辅助

| C++ 功能 | 迁移可能性 | 说明 |
|----------|-----------|------|
| `Debug::Print()` | ✅ 可完全迁移 | 纯 `GEngine->AddOnScreenDebugMessage` |
| `Debug::Log()` | ✅ 可完全迁移 | 纯 `UE_LOG` |

**结论**: **可完全迁移到 TS**。调试输出逻辑简单，TS 可直接调用 UE 的 C++ API。

---

## 三、TS 迁移总体策略

### 3.1 核心原则：C++ 骨架 + TS 业务逻辑

```
┌─────────────────────────────────────────────────────┐
│                    C++ 层（骨架）                     │
│  ┌──────────────────────────────────────────────┐   │
│  │ • UCLASS / GENERATED_BODY 宏声明             │   │
│  │ • UPROPERTY 属性声明（BindWidget/EditAnywhere│   │
│  │   /Config/Transient 等）                     │   │
│  │ • Native 虚函数覆写                          │   │
│  │   (NativePreConstruct, NativeOnHovered 等)   │   │
│  │ • UFUNCTION 声明                             │   │
│  │ • USubsystem / UGameInstance 生命周期        │   │
│  │ • UBlueprintAsyncActionBase 子类框架         │   │
│  │ • UE_DECLARE/DEFINE_GAMEPLAY_TAG             │   │
│  │ • Puerts FJsEnv 初始化                       │   │
│  └──────────────────────────────────────────────┘   │
│                         │                           │
│            BlueprintImplementableEvent               │
│            BlueprintCallable 接口暴露                │
│                         │                           │
│  ┌──────────────────────┴───────────────────────┐   │
│  │              TS 层（业务逻辑）                │   │
│  │  ┌──────────────────────────────────────┐    │   │
│  │  │ • 游戏流程控制（UI 状态切换等）       │    │   │
│  │  │ • 数据查询逻辑（设置读取等）          │    │   │
│  │  │ • 调试输出                           │    │   │
│  │  │ • UI 交互逻辑（按钮点击响应等）       │    │   │
│  │  │ • 音视频设置逻辑                      │    │   │
│  │  │ • 异步回调处理（推送控件后的回调）    │    │   │
│  │  └──────────────────────────────────────┘    │   │
│  └──────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────┘
```

### 3.2 迁移优先级分级

| 优先级 | 类/模块 | 可迁移内容 | 迁移方式 |
|--------|---------|-----------|---------|
| **P0（立即可迁移）** | `UFrontendFunctionLibrary` | 两个静态查询方法 | TS 重写，调用 C++ Settings API |
| **P0** | `FrontendDebugHelper` | `Print` / `Log` 函数 | TS 重写，调用 UE C++ API |
| **P1（可部分迁移）** | `UFrontendGameUserSettings` | Getter/Setter 方法 | TS 实现，通过 C++ 属性读写 |
| **P1** | `UFrontendLoadingScreenSettings` | 加载逻辑 | TS 实现，通过 C++ 属性 |
| **P1** | `UFrontendCommonButtonBase` | `SetButtonText` / `GetButtonDisplayText` | TS 调用 C++ 控件方法 |
| **P1** | `UWidget_ActivatableBase` | `GetOwningFrontendPlayerController` | TS 调用 |
| **P1** | `UWidget_PrimaryLayout` | `FindWidgetStackByTag` / `RegisterWidgetStack` | TS 调用 |
| **P1** | `UFrontendUISubsystem` | `Get` / `RegisterCreatedPrimaryLayout` | TS 调用 |
| **P2（需设计接口）** | `AFrontendPlayerController` | `OnPossess` 中相机逻辑 | C++ 暴露接口，TS 调用 |
| **P2** | `UAsyncAction_PushSoftWidget` | 回调中的焦点设置逻辑 | TS 绑定委托回调 |
| **P3（不建议迁移）** | `UFrontendDeveloperSettings` | 配置映射表声明 | 留在 C++，TS 只读 |
| **P3** | `UFrontendGameplayTags` | 标签定义 | 留在 C++，TS 字符串引用 |
| **P3** | `UMyGameInstance` | Puerts 初始化 | 留在 C++ |

---

## 四、详细迁移方案

### 4.1 P0 迁移方案

#### 4.1.1 `UFrontendFunctionLibrary` → TS

**当前 C++ 实现**:
```cpp
TSoftClassPtr<UWidget_ActivatableBase> UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FGameplayTag InWidgetTag)
{   
    const UFrontendDeveloperSettings* Settings = GetDefault<UFrontendDeveloperSettings>();
    checkf(Settings->FrontendWidgetMap.Contains(InWidgetTag), ...);
    return Settings->FrontendWidgetMap.FindRef(InWidgetTag);
}

TSoftObjectPtr<UTexture2D> UFrontendFunctionLibrary::GetOptionsSoftImageByTag(FGameplayTag InImageTag)
{
    const UFrontendDeveloperSettings* Settings = GetDefault<UFrontendDeveloperSettings>();
    checkf(Settings->OptionsScreenSoftImageMap.Contains(InImageTag), ...);
    return Settings->OptionsScreenSoftImageMap.FindRef(InImageTag);
}
```

**TS 迁移方案**:
```typescript
// FrontendFunctionLibrary.ts
import { $ref, $unref, $typeof } from 'puerts';

// 获取前端控件类
function getFrontendSoftWidgetClassByTag(widgetTag: string): any {
    const Settings = Unreal.GetDefault(Unreal.UFrontendDeveloperSettings);
    if (!Settings.FrontendWidgetMap.Contains(widgetTag)) {
        console.error(`Could not find the corresponding widget under the tag ${widgetTag}`);
        return null;
    }
    return Settings.FrontendWidgetMap.FindRef(widgetTag);
}

// 获取选项界面图片
function getOptionsSoftImageByTag(imageTag: string): any {
    const Settings = Unreal.GetDefault(Unreal.UFrontendDeveloperSettings);
    if (!Settings.OptionsScreenSoftImageMap.Contains(imageTag)) {
        console.error(`Could not find an image associated with tag ${imageTag}`);
        return null;
    }
    return Settings.OptionsScreenSoftImageMap.FindRef(imageTag);
}
```

#### 4.1.2 `FrontendDebugHelper` → TS

**当前 C++ 实现**:
```cpp
namespace Debug
{
    inline void Print(const FString& Message, int InKey = -1, const FColor& InColor = FColor::MakeRandomColor())
    {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(InKey, 7.f, InColor, Message); }
        UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
    }
    inline void Log(const FString& Message) { UE_LOG(LogTemp, Log, TEXT("%s"), *Message); }
}
```

**TS 迁移方案**:
```typescript
// DebugHelper.ts
namespace Debug {
    export function print(message: string, key: number = -1, color?: any): void {
        const engine = Unreal.GEngine;
        if (engine) {
            const inColor = color ?? Unreal.FColor.MakeRandomColor();
            engine.AddOnScreenDebugMessage(key, 7.0, inColor, message);
        }
        console.log(message);
    }
    
    export function log(message: string): void {
        console.log(message);
    }
}
```

---

### 4.2 P1 迁移方案

#### 4.2.1 `UFrontendGameUserSettings` Getter/Setter → TS

**需要在 C++ 中暴露的接口**:
```cpp
// 保持 C++ 属性声明不变，添加更多 BlueprintCallable 方法
UFUNCTION(BlueprintCallable)
float GetOverallVolume() const { return OverallVolume; }

UFUNCTION(BlueprintCallable)
void SetOverallVolume(float InVolume);

// ... 其他类似方法
```

**TS 迁移方案**:
```typescript
// GameUserSettings.ts
class FrontendGameUserSettings {
    private settings: any; // UFrontendGameUserSettings 实例
    
    constructor() {
        this.settings = Unreal.UFrontendGameUserSettings.Get();
    }
    
    // 音频控制
    get overallVolume(): number { return this.settings.GetOverallVolume(); }
    set overallVolume(v: number) { this.settings.SetOverallVolume(v); }
    
    get musicVolume(): number { return this.settings.GetMusicVolume(); }
    set musicVolume(v: number) { this.settings.SetMusicVolume(v); }
    
    get soundFXVolume(): number { return this.settings.GetSoundFXVolume(); }
    set soundFXVolume(v: number) { this.settings.SetSoundFXVolume(v); }
    
    get allowBackgroundAudio(): boolean { return this.settings.GetAllowBackgroundAudio(); }
    set allowBackgroundAudio(v: boolean) { this.settings.SetAllowBackgroundAudio(v); }
    
    get useHDRAudioMode(): boolean { return this.settings.GetUseHDRAudioMode(); }
    set useHDRAudioMode(v: boolean) { this.settings.SetUseHDRAudioMode(v); }
    
    // 视频控制
    get currentDisplayGamma(): number { return this.settings.GetCurrentDisplayGamma(); }
    set currentDisplayGamma(v: number) { this.settings.SetCurrentDisplayGamma(v); }
    
    // 游戏难度
    get currentGameDifficulty(): string { return this.settings.GetCurrentGameDifficulty(); }
    set currentGameDifficulty(v: string) { this.settings.SetCurrentGameDifficulty(v); }
}
```

#### 4.2.2 `UFrontendCommonButtonBase` 逻辑 → TS 调用

**需要在 C++ 中暴露的事件接口**:
```cpp
// 在头文件中添加
UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnButtonHovered")
void BP_OnButtonHovered();

UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnButtonUnhovered")
void BP_OnButtonUnhovered();

UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnButtonDescriptionUpdated")
void BP_OnButtonDescriptionUpdated(const FText& DescriptionText);
```

**C++ Native 函数调用蓝图事件**:
```cpp
void UFrontendCommonButtonBase::NativeOnHovered()
{
    Super::NativeOnHovered();
    if (!ButtonDescriptionText.IsEmpty())
    {
        BP_OnButtonDescriptionUpdated(ButtonDescriptionText);
    }
    BP_OnButtonHovered();
}
```

**TS 迁移方案**:
```typescript
// CommonButtonBase.ts
class FrontendCommonButtonBase {
    private widget: any; // UFrontendCommonButtonBase 实例
    
    constructor(buttonInstance: any) {
        this.widget = buttonInstance;
    }
    
    // 设置按钮文字
    setButtonText(text: string): void {
        this.widget.SetButtonText(text);
    }
    
    // 获取按钮显示文字
    getButtonDisplayText(): string {
        return this.widget.GetButtonDisplayText().ToString();
    }
    
    // 设置按钮图片
    setButtonDisplayImage(brush: any): void {
        this.widget.SetButtionDisplayImage(brush);
    }
}
```

#### 4.2.3 `UFrontendUISubsystem` 关键方法 → TS 调用

**TS 迁移方案**:
```typescript
// FrontendUISubsystem.ts
class FrontendUISubsystem {
    private static getInstance(worldContext?: any): any {
        return Unreal.UFrontendUISubsystem.Get(worldContext);
    }
    
    // 获取子系统实例
    static get(context?: any): any {
        return this.getInstance(context);
    }
    
    // 注册主布局
    static registerPrimaryLayout(layout: any, context?: any): void {
        const subsystem = this.getInstance(context);
        subsystem.RegisterCreatedPrimaryLayout(layout);
    }
    
    // 异步推送控件（复杂，建议保留 C++ 实现，TS 仅调用）
    static pushSoftWidget(
        widgetStackTag: string,
        softWidgetClass: any,
        onCreatedBeforePush: (widget: any) => void,
        afterPush: (widget: any) => void,
        context?: any
    ): void {
        const subsystem = this.getInstance(context);
        // 注意: PushSoftWidgetToStackAsync 涉及异步加载，
        // 建议通过蓝图异步动作 UAsyncAction_PushSoftWidget 调用
    }
}
```

---

### 4.3 P2 迁移方案

#### 4.3.1 `AFrontendPlayerController` 相机逻辑 → TS

**需要在 C++ 中暴露的接口**:
```cpp
UFUNCTION(BlueprintCallable)
void SetDefaultCamera();
```

**TS 迁移方案**:
```typescript
// PlayerController.ts
class FrontendPlayerController {
    private pc: any; // AFrontendPlayerController 实例
    
    constructor(playerController: any) {
        this.pc = playerController;
    }
    
    // 设置默认相机
    setDefaultCamera(): void {
        const allCameras = Unreal.UGameplayStatics.GetAllActorsOfClassWithTag(
            this.pc,
            Unreal.ACameraActor.StaticClass(),
            "Default"
        );
        if (allCameras.Num() > 0) {
            this.pc.SetViewTarget(allCameras[0]);
        }
    }
}
```

#### 4.3.2 `UAsyncAction_PushSoftWidget` 回调 → TS 监听

**TS 迁移方案**:
```typescript
// AsyncPushWidget.ts
class AsyncPushWidget {
    // 通过蓝图异步动作的委托来处理回调
    static pushSoftWidget(
        owningPlayerController: any,
        softWidgetClass: any,
        widgetStackTag: string,
        focusOnNewlyPushedWidget: boolean = true
    ): Promise<any> {
        return new Promise((resolve, reject) => {
            const action = Unreal.UAsyncAction_PushSoftWidget.PushSoftWidget(
                owningPlayerController,
                owningPlayerController,
                softWidgetClass,
                widgetStackTag,
                focusOnNewlyPushedWidget
            );
            
            action.OnWidgetCreatedBeforePush.Add((widget: any) => {
                // 推送前处理
            });
            
            action.AfterPush.Add((widget: any) => {
                resolve(widget);
            });
        });
    }
}
```

---

## 五、实施路线图

### 阶段 1: 准备工作（1-2 天）
- [ ] 确认 Puerts 版本和 TS 编译配置
- [ ] 在 `MyGameInstance` 中增强 Puerts 绑定（暴露必要的 C++ 类）
- [ ] 创建 TS 项目目录结构

### 阶段 2: P0 迁移（2-3 天）
- [ ] 将 `FrontendDebugHelper` 迁移到 TS
- [ ] 将 `UFrontendFunctionLibrary` 逻辑迁移到 TS
- [ ] 验证 TS 可正确调用 C++ Settings API
- [ ] 删除或简化 C++ 实现（保留必要的 `UFUNCTION` 声明）

### 阶段 3: P1 迁移（3-5 天）
- [ ] 为 `UFrontendCommonButtonBase` 添加 `BlueprintImplementableEvent` 接口
- [ ] 将 `UFrontendGameUserSettings` 的 Getter/Setter 迁移到 TS
- [ ] 将 `UFrontendLoadingScreenSettings` 的加载逻辑迁移到 TS
- [ ] 将 `UFrontendUISubsystem` 的常用方法封装为 TS API
- [ ] 将 `UWidget_ActivatableBase` / `UWidget_PrimaryLayout` 的查询逻辑迁移到 TS

### 阶段 4: P2 迁移（3-5 天）
- [ ] 为 `AFrontendPlayerController` 暴露 `SetDefaultCamera` 方法
- [ ] 为 `UAsyncAction_PushSoftWidget` 回调逻辑创建 TS 封装
- [ ] 集成测试：TS 调用链路完整性验证

### 阶段 5: 优化与文档（2-3 天）
- [ ] TS 代码 review 和性能优化
- [ ] 更新开发文档
- [ ] 回归测试

---

## 六、技术风险与应对

| 风险 | 影响 | 应对策略 |
|------|------|---------|
| Puerts 绑定不完整，部分 C++ 类型无法在 TS 中使用 | 高 | 逐步扩展绑定，对缺失类型提供 TS 包装器 |
| `FSlateBrush` 等复杂结构体在 TS 中构造困难 | 中 | 提供 C++ 工厂方法，TS 调用 |
| 异步操作（`RequestAsyncLoad`）回调在 TS 中处理复杂 | 中 | 保留 C++ 异步框架，TS 只处理回调 |
| TS 与 C++ 之间的类型转换开销 | 低 | 避免频繁跨边界调用，批量传递数据 |
| `GameplayTag` 在 TS 中的使用方式 | 中 | 使用字符串常量，通过 C++ 转换为 `FGameplayTag` |

---

## 七、最终结论

### 可迁移到 TS 的部分（约 35-40%）
- `UFrontendFunctionLibrary` 的全部逻辑
- `FrontendDebugHelper` 的全部逻辑
- `UFrontendGameUserSettings` 的 Getter/Setter
- `UFrontendLoadingScreenSettings` 的加载逻辑
- `UFrontendCommonButtonBase` 的业务方法（通过 C++ 接口调用）
- `UWidget_ActivatableBase` / `UWidget_PrimaryLayout` 的查询方法
- `UFrontendUISubsystem` 的常用方法
- `AFrontendPlayerController` 的相机设置逻辑
- `UAsyncAction_PushSoftWidget` 的回调处理

### 必须保留在 C++ 的部分（约 60-65%）
- 所有 `UCLASS` / `GENERATED_BODY` 宏声明
- 所有 `UPROPERTY` 属性声明（`BindWidget`、`EditAnywhere`、`Config` 等）
- 所有 Native 虚函数覆写（`NativePreConstruct`、`NativeOnHovered` 等）
- `UFrontendDeveloperSettings` 的配置映射表
- `UFrontendGameplayTags` 的标签定义
- `UMyGameInstance` 的 Puerts 初始化
- `UAsyncAction_PushSoftWidget` 的异步加载框架
- `UFrontendUISubsystem` 的异步推送框架

### 推荐架构
**C++ 骨架 + 蓝图事件 + TS 业务逻辑**：C++ 负责框架和声明，TS 负责逻辑和流程控制，蓝图负责 UI 布局和可视化编辑。
