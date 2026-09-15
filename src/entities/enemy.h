#pragma once

#include "entities/character.h"

#include <string>
#include <vector>

struct LootItem {
    std::string itemName; //掉落物品名称
    int minAmout; //最少掉落物品数量
    int maxAmout; //最多掉落物品数量
    float dropChance; //掉落概率，0.0F - 1.0
};

// 敌方角色的基础类，保存索敌和掉落配置；具体种类由后续关卡数据确定。
class Enemy : public Character {

public:
    // name 是敌军实体名称。
    void update(float deltaTime, float worldGravity) override; //敌人状态更新
    
    float getDetectionRange() const; // 返回当前兵种用于选择目标的水平检测距离

    std::vector<LootItem> dropLoot(); //掉落物品表

protected:
    explicit Enemy(std::string name);

    float detectionRange_;     // 兵种自身索敌距离，供后续 AI 或技能配置使用

    std::vector<LootItem> lootTable_; //掉落物品表

};
