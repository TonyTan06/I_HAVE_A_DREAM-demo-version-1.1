#pragma once

#include <string>

enum class ItemRarity {
    Common,     // 普通 白色
    Uncommon,   // 优良 绿色
    Rare,       // 稀有 蓝色
    Epic,       // 史诗 紫色
    Legendary,  // 传说 金色
    Mythic,     // 神话 红色
    Special,    // 特殊 彩色
    Unique      // 独特 黑色
};

// 可进入背包、商店或掉落系统的基础物品数据。
class Item {
public:
    Item(std::string name, ItemRarity rarity, int basePrice); //填写物品名字，稀有度，基础价格

    const std::string& getName() const; //获取物品名字
    ItemRarity getRarity() const; //获取物品稀有度
    std::string getRarityName() const; //获取物品稀有度的字符串表示
    float getRarityMultiplier() const; //获取物品稀有度的价格倍率   
    int getBasePrice() const; //获取物品基础价格
    int getSellPrice() const; //获取物品出售价格，基础价格 * 稀有度倍率

private:
    std::string name_; // 物品显示名称
    ItemRarity rarity_; // 决定售价倍率的稀有度枚举
    int basePrice_; // 未乘稀有度倍率前的基础售价
};
