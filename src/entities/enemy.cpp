#include "entities/enemy.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <random>

Enemy::Enemy(std::string name)
    : Character(std::move(name)),

      detectionRange_(0.0F) //检测范围

{
    setGravityScale(1.0F);
    setFaction(Faction::Enemy);
    isFacingRight_ = false; // 默认朝左，方便从右侧进入场景的敌军直接面向玩家
}

std::vector<LootItem> Enemy::dropLoot() {
    std::vector<LootItem> droppedItems; // 存储本次实际掉落的物品

    static std::random_device rd; // 创建随机数种子（只初始化一次）
    static std::mt19937 generator(rd()); // Mersenne Twister 随机数引擎（只初始化一次）

    std::uniform_real_distribution<float> chanceDistribution(0.0F, 1.0F); // 生成 [0.0, 1.0] 范围内的随机浮点数

    // 遍历当前敌人的掉落表
    for (const auto& item : lootTable_) {
        float roll = chanceDistribution(generator); // 为当前物品进行一次掉落判定

        if (roll <= item.dropChance) droppedItems.push_back(item); // 如果随机数小于等于掉落概率，则掉落该物品
    }

    // 返回最终掉落结果
    return droppedItems;
}

void Enemy::update(float deltaTime, float worldGravity)
{
    
    Character::update(deltaTime, worldGravity);
}


float Enemy::getDetectionRange() const
{
    return detectionRange_;
}

