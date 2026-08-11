#pragma once

#include "entities/character.h"
#include <string>

class Player : public Character {
public:
    explicit Player(std::string name); //创建了主角
    void update(float deltaTime, float worldGravity) override; //主角状态更新    
    void applyMeleeHit(Character& target) const; // 结算一次已经成功起手的近战命中
    void setDefending(bool shouldDefend); // 根据动作请求尝试进入或退出防御状态

    bool isDefending() const; // 当前是否处于可抵挡一次攻击的防御状态
    bool isDodging() const; // 当前是否正处于 0.2 秒闪避过程
    bool isDodgeCoolingDown() const; // 闪避是否仍在 5 秒冷却内
    bool consumeRangedAttackRequest(); // 供场景读取并清除本帧的子弹生成请求
    bool blockNextAttack(); // 防御判定命中后抵挡一次攻击并开始冷却
    bool startDodge(bool dodgeRight); // 开始一次沿指定方向的闪避
    bool tryBeginMeleeAttack() override; // 防御中或公共近战冷却未完成时拒绝起手
    bool tryBeginRangedAttack() override; // 防御中或公共远程冷却未完成时拒绝起手
    bool rangedAttack(); // 成功开始远程攻击时请求本帧生成子弹

    float getDodgeCooldownProgress() const; // 冷却完成比例，0 表示刚开始，1 表示刚结束
    float getLastDodgeDistance() const; // 上一帧由闪避产生的实际水平移动距离
    float getDefenseRange() const; // 防御判定的有效范围，单位：像素

    // 当前测试素材由两张 96×128 的竖向帧组成，碰撞箱与单帧尺寸一致。
    static constexpr float SPRITE_FRAME_WIDTH = 96.0F;
    static constexpr float SPRITE_FRAME_HEIGHT = 128.0F;

protected:
    // 供 PlayerShadow 在生成时继承玩家的跳跃次数和当前朝向。
    void copyMovementStateFrom(const Player& player);
    void copyAttributesFrom(const Player& player); // 供 PlayerShadow 在生成时继承玩家的战斗属性

private:

    bool rangedAttackRequested_; // 本帧是否需要由场景生成子弹
    bool isDefending_; // 持续请求防御且不在防御冷却时为 true
    bool isDodging_; // 闪避动作开始后、持续时间未结束时为 true
    bool dodgeRight_; // 本次闪避锁定的方向：true 向右，false 向左
    bool isTakingTrueDamage_; // 受到真实伤害时为 true，供场景播放受击特效使用

    float defenseCooldown_; // 成功防御后剩余的不可防御时间，单位：秒
    float dodgeElapsedTime_; // 当前闪避已进行的时间，单位：秒
    float dodgeCooldown_; // 下次可以闪避前的剩余时间，单位：秒
    float lastDodgeDistance_; // 最近一次 update 中闪避实际移动的像素距离
    float defenseRange_; // 防御判定的有效范围，单位：像素

    static constexpr float DEFENSE_COOLDOWN = 2.0F; // 成功抵挡一次攻击后的冷却
    static constexpr float DODGE_DURATION = 0.2F; // 完成一次闪避所需的时间
    static constexpr float DODGE_COOLDOWN = 5.0F; // 两次闪避之间的冷却

};
