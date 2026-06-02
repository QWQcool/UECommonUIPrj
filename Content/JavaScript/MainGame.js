"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const GameplayTags_1 = require("./GameplayTags");
const UE = require("ue");
class MainGame {
    constructor() {
        console.log('MainGame initialized');
        this.initializeGameplayTags();
    }
    initializeGameplayTags() {
        // 演示Gameplay Tags功能
        (0, GameplayTags_1.demonstrateGameplayTags)();
        // 实际使用Gameplay Tags的示例
        this.setupWidgetStackSystem();
    }
    setupWidgetStackSystem() {
        console.log("\n=== Setting up Widget Stack System ===");
        // 使用Gameplay Tags来确定Widget的堆栈位置
        const currentWidgetTag = GameplayTags_1.FrontendGameplayTags.Frontend_WidgetStack_GameHud;
        // 检查Widget应该放在哪个堆栈中
        if (GameplayTags_1.FrontendGameplayTags.containsTag(GameplayTags_1.FrontendGameplayTags.WidgetStackTags, currentWidgetTag)) {
            console.log(`Widget with tag '${UE.BlueprintGameplayTagLibrary.GetTagName(currentWidgetTag)}' ` +
                `can be placed in the widget stack system.`);
        }
        // 比较两个Tags
        const tag1 = GameplayTags_1.FrontendGameplayTags.Frontend_WidgetStack_Modal;
        const tag2 = GameplayTags_1.FrontendGameplayTags.Frontend_WidgetStack_Modal;
        const areEqual = UE.BlueprintGameplayTagLibrary.EqualEqual_GameplayTag(tag1, tag2);
        console.log(`Are tags equal: ${areEqual}`);
        // 创建自定义Tag容器
        const tagArray = UE.NewArray(UE.GameplayTag);
        tagArray.Add(GameplayTags_1.FrontendGameplayTags.Frontend_WidgetStack_GameHud);
        tagArray.Add(GameplayTags_1.FrontendGameplayTags.Frontend_WidgetStack_GameMenu);
        const myTags = UE.BlueprintGameplayTagLibrary.MakeGameplayTagContainerFromArray(tagArray);
        console.log(`My tag container has ${UE.BlueprintGameplayTagLibrary.GetNumGameplayTagsInContainer(myTags)} tags`);
    }
}
// 创建主游戏实例
const mainGame = new MainGame();
console.log("\nGameplay Tags are now available in TypeScript!");
//# sourceMappingURL=MainGame.js.map