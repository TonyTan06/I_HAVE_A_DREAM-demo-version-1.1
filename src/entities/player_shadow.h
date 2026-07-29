#pragma once

#include "entities/player.h"

// 技能属于具体影子实体；对局配置系统后续可以在生成后设置选定技能。
enum class ShadowSkill {
    SynchronizePlayerActions = 1, // 技能 1：复刻玩家的全部控制输入
    SwapPosition = 2 // 技能 2：与玩家交换位置和垂直物理状态
};

// PlayerShadow 继承 Player，因此能够复用移动、跳跃、防御、闪避和攻击请求。
// 它不处理生成与销毁；这些生命周期职责由 ShadowManager 负责。
class PlayerShadow : public Player {
public:
    // 在记录点创建影子，并快照玩家当下的战斗与跳跃属性。
    PlayerShadow(const Player& player, float x, float y);

    // 影子没有血量概念，任何直接伤害调用都会被忽略。
    void takeDamage(float damage) override;
    void update(float deltaTime) override; // 更新影子自身动作状态与技能 2 冷却
    void setActiveSkill(ShadowSkill skill); // 保存新技能并立即应用阵营等持续状态
    ShadowSkill getActiveSkill() const; // 返回当前影子正在使用的技能
    bool applyInput(const PlayerInputState& input,
                    HorizontalInputDirection horizontalDirection,
                    float deltaTime) override; // 根据 activeSkill_ 执行本帧技能
    // 返回 true 表示成功交换；冷却中返回 false，不改变双方状态。
    bool tryUsePositionSwap(Player& player);
    bool isPositionSwapCoolingDown() const; // 技能 2 是否仍在 10 秒冷却内
    float getPositionSwapCooldownProgress() const; // 冷却完成比例，范围 0 到 1

private:
    ShadowSkill activeSkill_; // 当前实体技能，测试阶段默认使用技能 1
    float positionSwapCooldown_; // 技能 2 剩余冷却时间，单位：秒

    static constexpr float POSITION_SWAP_COOLDOWN = 10.0F; // 技能 2 固定冷却时间

    void applyActiveSkillState(); // 根据 activeSkill_ 应用阵营等持续状态
};
