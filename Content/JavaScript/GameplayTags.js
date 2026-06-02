"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.FrontendGameplayTags = void 0;
exports.demonstrateGameplayTags = demonstrateGameplayTags;
const UE = require("ue");
const puerts = require("puerts");
/**
 * Frontend Gameplay Tags for TypeScript
 * 对应于C++中的FrontendGameplayTags
 */
class FrontendGameplayTags {
    // Widget Stack Tags - 使用getter确保在使用时才创建
    static get Frontend_WidgetStack_Modal() {
        return new UE.GameplayTag("Frontend.WidgetStack.Modal");
    }
    static get Frontend_WidgetStack_GameMenu() {
        return new UE.GameplayTag("Frontend.WidgetStack.GameMenu");
    }
    static get Frontend_WidgetStack_GameHud() {
        return new UE.GameplayTag("Frontend.WidgetStack.GameHud");
    }
    static get Frontend_WidgetStack_Frontend() {
        return new UE.GameplayTag("Frontend.WidgetStack.Frontend");
    }
    // 创建包含所有Widget Stack Tags的容器 - 使用getter
    static get WidgetStackTags() {
        // 创建TArray并添加所有Tags
        const tagArray = UE.NewArray(UE.GameplayTag);
        tagArray.Add(FrontendGameplayTags.Frontend_WidgetStack_Modal);
        tagArray.Add(FrontendGameplayTags.Frontend_WidgetStack_GameMenu);
        tagArray.Add(FrontendGameplayTags.Frontend_WidgetStack_GameHud);
        tagArray.Add(FrontendGameplayTags.Frontend_WidgetStack_Frontend);
        return UE.BlueprintGameplayTagLibrary.MakeGameplayTagContainerFromArray(tagArray);
    }
    // 辅助方法：验证Tag是否有效
    static isValid(tag) {
        return UE.BlueprintGameplayTagLibrary.IsGameplayTagValid(tag);
    }
    // 辅助方法：检查容器是否包含特定Tag
    static containsTag(container, tag, exactMatch = true) {
        return UE.BlueprintGameplayTagLibrary.HasTag(container, tag, exactMatch);
    }
    // 辅助方法：获取所有Tag名称
    static getAllTagNames() {
        const tags = [];
        // 获取容器中的所有Tag - 使用UE.NewArray
        const tagArray = UE.NewArray(UE.GameplayTag);
        UE.BlueprintGameplayTagLibrary.BreakGameplayTagContainer(FrontendGameplayTags.WidgetStackTags, puerts.$ref(tagArray));
        for (let i = 0; i < tagArray.Num(); i++) {
            const tag = tagArray.Get(i);
            tags.push(UE.BlueprintGameplayTagLibrary.GetTagName(tag));
        }
        return tags;
    }
}
exports.FrontendGameplayTags = FrontendGameplayTags;
// 使用示例
function demonstrateGameplayTags() {
    console.log("=== Gameplay Tags Demo ===");
    // 使用常量Tag
    const modalTag = FrontendGameplayTags.Frontend_WidgetStack_Modal;
    console.log(`Modal Tag: ${UE.BlueprintGameplayTagLibrary.GetTagName(modalTag)}`);
    console.log(`Is valid: ${FrontendGameplayTags.isValid(modalTag)}`);
    // 检查容器
    console.log(`Container contains Modal: ${FrontendGameplayTags.containsTag(FrontendGameplayTags.WidgetStackTags, modalTag)}`);
    // 获取所有Tag名称
    const allTags = FrontendGameplayTags.getAllTagNames();
    console.log(`All Tags: ${allTags.join(', ')}`);
}
//# sourceMappingURL=GameplayTags.js.map