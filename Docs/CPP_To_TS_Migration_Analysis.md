# C++ 到 TypeScript 迁移可行性分析文档

> 项目：UECommonUIPrj  
> 日期：2026-06-03  
> 目的：分析现有 C++ 逻辑迁移到 TypeScript 的可行性，并规划迁移路径

---

## 目录

1. [项目现状概述](#1-项目现状概述)
2. [C++ 代码分类与迁移可行性](#2-c-代码分类与迁移可行性)
3. [无法迁移的部分（必须保留 C++）](#3-无法迁移的部分必须保留-c)
4. [可迁移到 TypeScript 的部分](#4-可迁移到-typescript-的部分)
5. [可通过蓝图辅助实现的部分](#5-可通过蓝图辅助实现的部分)
6. [迁移策略建议](#6-迁移策略建议)
7. [迁移路线图](#7-迁移路线图)

---

## 1. 项目现状概述

### 当前 C++ 模块结构

```
Source/UEAdvancedUI/
├── Public/
│   ├── FrontendGameplayTags.h          // GameplayTag 声明
│   ├── FrontendDebugHelper.h           // 调试辅助工具
│   ├── MyGameInstance.h               // GameInstance（已集成 Puerts）
│   ├── Controllers/
│   │   └── FrontendPlayerController.h  // 前端玩家控制器
│   ├── Subsystems/
│   │   └── FrontendUISubsystem.h      // UI 子系统（GameInstance 级别）
│   ├── Widgets/
│   │   ├── Widget_PrimaryLayout.h      // 主布局 Widget
│   │   └── Widget_ActivatableBase.h   // 可激活 Widget 基类
│   └── AsyncActions/
│       └── AsyncAction_PushSoftWidget.h // 蓝图异步操作节点
└── Private/
    ├── UEAdvancedUI.cpp/h
    ├── FrontendGameplayTags.cpp         // GameplayTag 定义
    ├── MyGameInstance.cpp
    ├── Controllers/
    │   └── FrontendPlayerController.cpp
    ├── Subsystems/
    │   └── FrontendUISubsystem.cpp
    ├── Widgets/
    │   ├── Widget_PrimaryLayout.cpp
    │   └── Widget_ActivatableBase.cpp
    └── AsyncActions/
        └── AsyncAction_PushSoftWidget.cpp
```

### 当前 TypeScript 现状

```
TypeScript/
└── MainGame.ts  // 入口文件（目前基本为空）
```

---

## 2. C++ 代码分类与迁移可行性

### 2.1 分类标准

| 类别 | 描述 | 迁移可行性 |
|------|------|-----------|
| **A. 编辑器/运行时前置代码** | 引擎启动/编辑器加载时就需要生效的代码 | ❌ 不可迁移 |
| **B. 反射系统依赖代码** | 依赖 UPROPERTY/UFUNCTION 等反射宏 | ⚠️ 部分可迁移 |
| **C. 纯逻辑代码** | 不依赖 UObject 生命周期的纯逻辑 | ✅ 可迁移 |
| **D. UObject 派生类** | UClass/UWidget 等 UObject 派生类 | ⚠️ 需保留 C++ 基类 |
| **E. 异步操作节点** | 蓝图节点（AsyncAction） | ✅ 可用 TS + 蓝图混合 |

---

## 3. 无法迁移的部分（必须保留 C++）

### 3.1 GameplayTag 声明（Native Tag）

**文件：** `FrontendGameplayTags.h/cpp`

```cpp
// Header - 外部声明
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_Modal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_GameMenu)
// ...

// CPP - 实际定义（注册为 Native Tag）
UE_DEFINE_GAMEPLAY_TAG(Frontend_WidgetStack_Modal, "Frontend.WidgetStack.Modal");
```

**无法迁移原因：**
- `UE_DEFINE_GAMEPLAY_TAG` 宏在**模块加载时**就将 Tag 注册为 **Native Gameplay Tag**
- TypeScript 是**运行时**才执行的，此时编辑器已经启动，Tag Manager 已经初始化
- 结果：在 Tag Manager 中看不到这些 Tag，蓝图也无法使用

**替代方案：**
- ✅ **保留 C++ 声明**（推荐用于核心 Tag）
- ✅ **使用 INI 配置文件**（`DefaultGameplayTags.ini`）
- ❌ TypeScript 无法替代

---

### 3.2 UClass 派生类的声明（UWidget、USubsystem 等）

**文件：** 所有 `UCLASS()` 标记的类

```cpp
UCLASS()
class UEADVANCEDUI_API UWidget_PrimaryLayout : public UCommonUserWidget { ... };

UCLASS()
class UEADVANCEDUI_API UFrontendUISubsystem : public UGameInstanceSubsystem { ... };
```

**无法迁移原因：**
- UE 需要 C++ 类来生成 `.generated.h` 反射信息
- 蓝图继承自 C++ 基类，不能继承自 TS 类
- `UCLASS()` 宏在编译期生成 UHT 代码

**替代方案：**
- ✅ **保留 C++ 基类**（最小 C++ 壳类）
- ✅ **业务逻辑用 TS 实现**（通过 Puerts 的 `extends` 或 Mixin）

---

### 3.3 模块入口（IPuertsModule 等）

**文件：** `UEAdvancedUI.cpp/h`、`UEAdvancedUI.Build.cs`

**无法迁移原因：**
- 模块加载依赖 `.Build.cs` 文件
- Puerts 的 JsEnv 需要在 C++ 中初始化

**替代方案：**
- ✅ 保留最小 C++ 入口
- ✅ TS 只负责业务逻辑

---

## 4. 可迁移到 TypeScript 的部分

### 4.1 纯逻辑函数

**示例：** `FrontendUISubsystem::PushSoftWidgetToStackAsync` 中的逻辑

```cpp
// C++ 中的纯逻辑部分
void UFrontendUISubsystem::PushSoftWidgetToStackAsync(...)
{
    // 这部分逻辑可以迁移到 TS
    UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(...);
    // ...
}
```

**迁移方式：**

```typescript
// TypeScript 版本
export function PushSoftWidgetToStackAsync(
    widgetStackTag: UE.GameplayTag,
    softWidgetClass: UE.TSoftClassPtr<UE.Widget_ActivatableBase>,
    callback: (state: AsyncPushWidgetState, widget: UE.Widget_ActivatableBase) => void
) {
    const assetManager = UE.AssetManager.Get();
    assetManager.GetStreamableManager().RequestAsyncLoad(
        softWidgetClass.ToSoftObjectPath(),
        // ... TypeScript 实现
    );
}
```

---

### 4.2 调试辅助函数

**文件：** `FrontendDebugHelper.h`

```cpp
// C++ 版本
namespace Debug {
    static void Print(const FString& Message, const FColor& Color = FColor::White);
}
```

**迁移方式：**

```typescript
// TypeScript 版本
export namespace Debug {
    export function Print(message: string, color?: UE.LinearColor) {
        console.log(message);
        // 可选：调用 UE 的 GEngine->AddOnScreenDebugMessage
    }
}
```

---

### 4.3 事件绑定和 UI 交互逻辑

**可迁移内容：**
- Button 点击事件处理
- Widget 显示/隐藏逻辑
- 数据绑定逻辑

**迁移方式：** 使用 Puerts 的 `on` 函数绑定 UE 委托

```typescript
// TypeScript 版本
button.OnClicked.AddDynamic(() => {
    console.log("Button Clicked!");
});
```

---

## 5. 可通过蓝图辅助实现的部分

### 5.1 GameplayTag 配置

**C++ 方式：**
```cpp
UE_DEFINE_GAMEPLAY_TAG(Frontend_WidgetStack_Modal, "Frontend.WidgetStack.Modal");
```

**蓝图/INI 替代方式：**

创建 `Config/DefaultGameplayTags.ini`：

```ini
[/Script/GameplayTags.GameplayTagsSettings]
+GameplayTagList=(Tag="Frontend.WidgetStack.Modal",DevComment="模态窗口层")
+GameplayTagList=(Tag="Frontend.WidgetStack.GameMenu",DevComment="游戏菜单层")
+GameplayTagList=(Tag="Frontend.WidgetStack.GameHud",DevComment="游戏HUD层")
+GameplayTagList=(Tag="Frontend.WidgetStack.Frontend",DevComment="前端界面层")
```

**优点：**
- 不需要重新编译 C++
- 策划可以直接修改

---

### 5.2 Widget 堆栈注册

**当前 C++ 方式：**
```cpp
void UWidget_PrimaryLayout::RegisterWidgetStack(FGameplayTag StackTag, ...)
```

**蓝图替代方式：**
- 在蓝图中直接调用 `RegisterWidgetStack` 节点
- 不需要迁移到 TS，蓝图可视化更直观

---

### 5.3 AsyncAction 蓝图节点

**当前 C++ 方式：** `UAsyncAction_PushSoftWidget`

**TS + 蓝图混合方式：**

保留 C++ 的 `UAsyncAction_PushSoftWidget` 壳（因为蓝图节点必须是 C++ 类），但：
- 实际逻辑用 TS 实现
- 通过 Puerts 调用 TS 函数

**或者完全用 TS 实现异步加载：**

```typescript
// TypeScript 实现异步加载 Widget
export async function PushWidgetAsync(widgetClassPath: string, stackTag: string) {
    const assetManager = UE.AssetManager.Get();
    // 使用 Puerts 的异步加载 API
    // ...
}
```

然后在蓝图中调用 TS 函数（通过 `BlueprintCallable` 的 TS 函数）。

---

## 6. 迁移策略建议

### 6.1 推荐架构（混合模式）

```
┌─────────────────────────────────────────────────────────────┐
│                    Unreal Engine 编辑器                     │
├─────────────────────────────────────────────────────────────┤
│  C++ 层（最小壳类）                                        │
│  ├── FrontendGameplayTags.h/cpp  (保留 Native Tag 声明)    │
│  ├── Widget_ActivatableBase.h       (保留 UClass 基类)     │
│  ├── Widget_PrimaryLayout.h/cpp     (保留 UClass 基类)     │
│  └── FrontendUISubsystem.h          (保留 UClass 基类)     │
├─────────────────────────────────────────────────────────────┤
│  蓝图层（可视化编辑）                                       │
│  ├── BP_PrimaryLayout (继承自 Widget_PrimaryLayout)        │
│  └── 使用 TS 暴露的 BlueprintCallable 函数                 │
├─────────────────────────────────────────────────────────────┤
│  TypeScript 层（业务逻辑）                                 │
│  ├── MainGame.ts                  (入口)                   │
│  ├── GameplayTags.ts              (运行时 Tag 操作)         │
│  ├── UI/                          (UI 逻辑)               │
│  └── Utils/                        (工具函数)              │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 各层职责

| 层级 | 职责 | 是否可迁移到 TS |
|------|------|------------------|
| **C++ 层** | UClass 声明、Native Tag、模块入口 | ❌ 不可迁移 |
| **蓝图层** | 可视化布局、动画、简单逻辑 | ⚠️ 部分可迁移 |
| **TypeScript 层** | 复杂业务逻辑、网络请求、数据处理 | ✅ 完全用 TS |

---

## 7. 迁移路线图

### 阶段 1：保留 C++ 壳类（当前阶段）

**目标：** 保持现有 C++ 功能，建立 TS 开发环境

- [x] 集成 Puerts 插件
- [x] 创建 `MyGameInstance` 初始化 JsEnv
- [ ] 完善 `MainGame.ts` 入口文件
- [ ] 将调试工具迁移到 TS

---

### 阶段 2：迁移纯逻辑函数到 TS

**目标：** 将不依赖 UObject 生命周期的纯逻辑迁移到 TS

- [ ] 迁移 `FrontendUISubsystem` 中的异步加载逻辑到 TS
- [ ] 迁移调试辅助函数到 TS
- [ ] 在 C++ 中保留壳函数，内部调用 TS 实现

**示例：**

```cpp
// C++ 壳函数
void UFrontendUISubsystem::PushSoftWidgetToStackAsync(...)
{
    // 调用 TypeScript 实现
    if (JsEnv.IsValid())
    {
        JsEnv->GetService_Public()->Eval("require('UI/WidgetStack').PushSoftWidgetToStackAsync(...)");
    }
}
```

---

### 阶段 3：使用 Blueprint Mixin 替代部分 C++ 逻辑

**目标：** 使用 Puerts 的 `blueprint.registermixin` 替代 C++ 中的业务逻辑

- [ ] 将 `Widget_ActivatableBase` 的业务逻辑迁移到 TS
- [ ] 使用 Mixin 将 TS 逻辑注入蓝图

**示例：**

```typescript
// MainGame.ts
blueprint.registermixin(
    '/Game/Blueprints/UI/BP_ActivatableWidget.BP_ActivatableWidget_C',
    "UI/ActivatableWidgetTS"
);
```

---

### 阶段 4：评估是否可以进一步减少 C++ 代码

**目标：** 评估是否可以将更多 C++ 功能迁移到 TS

**可评估项：**
- `AsyncAction_PushSoftWidget` 是否可以用纯 TS 实现？
- `FrontendPlayerController` 的逻辑是否可以迁移到 TS？
- UI 逻辑是否可以用 TS + UMG 蓝图混合？

---

## 8. 总结

### 8.1 必须保留 C++ 的部分

| 内容 | 原因 |
|------|------|
| `FrontendGameplayTags` Native Tag 声明 | 编辑器启动时需要 |
| `UCLASS()` 派生类声明 | UE 反射系统要求 |
| `.Build.cs` 模块定义 | 编译系统要求 |
| `MyGameInstance` JsEnv 初始化 | Puerts 要求 |

### 8.2 可迁移到 TS 的部分

| 内容 | 迁移难度 |
|------|----------|
| 调试辅助函数 | ⭐ 简单 |
| 异步加载逻辑 | ⭐⭐ 中等 |
| UI 交互逻辑 | ⭐⭐ 中等 |
| 事件绑定 | ⭐ 简单 |

### 8.3 建议

1. **保留 C++ 壳类**，不要尝试完全删除 C++
2. **优先迁移纯逻辑函数**，这些最容易迁移
3. **使用 Blueprint Mixin** 来替代 C++ 中的业务逻辑
4. **GameplayTag 保留 C++ 声明**，这是最稳定的方式
5. **逐步迁移**，不要一次性迁移所有代码

---

## 9. 附录：Puerts 能力清单

### 9.1 Puerts 可以做的事

- ✅ 继承 UObject 派生类（`extends UE.GameInstance`）
- ✅ 重写 UFUNCTION 虚函数
- ✅ 绑定 UE 委托（`OnClicked.AddDynamic`）
- ✅ 调用 UE C++ 函数
- ✅ 使用 Blueprint Mixin 注入蓝图逻辑

### 9.2 Puerts 不能做的事

- ❌ 定义新的 `UCLASS()`（必须在 C++ 中定义）
- ❌ 定义新的 `UPROPERTY()`（必须在 C++ 中定义）
- ❌ 注册 Native GameplayTag（必须在 C++ 中或 INI 中）
- ❌ 替代 `.Build.cs` 模块定义

---

> **结论：** 完全迁移到 TypeScript 是不现实的，但采用 **C++ 壳类 + TypeScript 业务逻辑** 的混合架构是非常可行且推荐的方案。
