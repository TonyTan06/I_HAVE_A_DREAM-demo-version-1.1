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

// 所有敌方兵种的公共基类：保存阵营、等级、索敌、朝向、攻击冷却和复活状态。
// MeleeEnemy 与 RangedEnemy 只需要设置各自数值，不再重复实现这些公共行为。
class Enemy : public Character {

public:
    // name 是敌军实体名称；level 最低按 1 级处理，并用于计算击杀经验。
    explicit Enemy(std::string name, int level = 1);

    void attack() override; // 冷却允许时输出一次通用攻击信息
    bool tryAttack(); // 尝试消耗一次攻击冷却，不直接指定攻击表现
    bool attack(Character& target); // 对非同阵营目标造成伤害
    void update(float deltaTime) override; //敌人状态更新
    bool isRespawning() const; // 当前是否正处于死亡后的复活倒计时
    float getRespawnProgress() const; // 复活倒计时的完成比例，范围 0 到 1
    int getLevel() const; // 返回当前敌军等级，最低为 1 级
    int getExperienceReward() const; // 返回击杀该敌军可获得的经验值
    float getDetectionRange() const; // 返回当前兵种用于选择目标的水平检测距离
    bool isFacingRight() const; // 返回敌军当前水平朝向，默认朝左
    // 只比较水平坐标；目标与敌军 x 相同时保持原朝向，防止画面抖动。
    void faceToward(const Character& target);

    std::vector<LootItem> dropLoot(); //掉落物品表

protected:
    float detectionRange_;     // 发现玩家距离
    float attackRange_;        // 兵种自身攻击距离，供后续 AI 或技能配置使用
    int level_;                // 敌军等级，用于计算击杀经验等奖励数值
    int experienceReward_;     // 击杀获得经验，当前公式为 5 × 敌军等级
    bool isAggro_;             // 是否进入仇恨状态
    bool isFacingRight_;       // true 表示朝右，false 表示朝左

    float attackCooldown_; // 攻击冷却时间
    float timeSinceLastAttack_; // 自上次攻击以来的时间
    bool isRespawning_; // 死亡后第一次 update 起设为 true
    float respawnElapsedTime_; // 本次复活倒计时已过去的时间，单位：秒
    static constexpr float RESPAWN_DURATION = 5.0F; // 敌军从死亡到满血复活所需时间
    void resetAttackCooldown(); // 成功攻击或复活时把攻击计时归零

    std::vector<LootItem> lootTable_; //掉落物品表

};
