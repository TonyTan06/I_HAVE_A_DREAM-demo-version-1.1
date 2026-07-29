#pragma once

#include "character.h"
#include "player_controller.h"
#include <string>

class Player : public Character {
public:
    explicit Player(std::string name); //创建了主角

    void moveRight(float deltaTime); //右移
    void moveLeft(float deltaTime); //左移
    void jump() override; //跳跃
    void update(float deltaTime) override; //主角状态更新
    void addGold(int value); //获得金币
    void addExperience(int value); //获得经验
    void levelUp(); //主角升级
    void attack() override;
    void attack(Character& target); // 对目标造成当前攻击伤害
    void rangedAttack() override; // 请求一次远程攻击，受自身攻击间隔限制
    bool isFacingRight() const; // 当前朝向，供眼睛与近战攻击判定使用
    bool isMovingHorizontally() const; // 当前输入是否会显示行走姿势
    bool consumeRangedAttackRequest(); // 供场景读取并清除本帧的子弹生成请求
    void setDefending(bool shouldDefend); // 按住防御键时尝试进入防御状态
    bool isDefending() const; // 当前是否处于可抵挡一次攻击的防御状态
    bool blockNextAttack(); // 防御判定命中后抵挡一次攻击并开始冷却
    bool startDodge(bool dodgeRight); // 开始一次沿指定方向的闪避
    bool isDodging() const; // 当前是否正处于 0.2 秒闪避过程
    bool isDodgeCoolingDown() const; // 闪避是否仍在 5 秒冷却内
    float getDodgeCooldownProgress() const; // 冷却完成比例，0 表示刚开始，1 表示刚结束
    float getLastDodgeDistance() const; // 上一帧由闪避产生的实际水平移动距离
    // 把一帧设备无关输入应用到角色；返回本帧是否请求近战攻击。
    virtual bool applyInput(const PlayerInputState& input,
                            HorizontalInputDirection horizontalDirection,
                            float deltaTime);

    int getJumpCount() const; //跳跃次数（可以实现二连跳）
    int getMaxJumpCount() const; //最大跳跃次数
    int getGold() const; 
    int getExperience() const;
    int getLevel() const;
    int getExperienceThreshold() const; //升级需要的经验

    // 当前测试素材由两张 96×128 的竖向帧组成，碰撞箱与单帧尺寸一致。
    static constexpr float SPRITE_FRAME_WIDTH = 96.0F;
    static constexpr float SPRITE_FRAME_HEIGHT = 128.0F;

private:
    int jumpCount_; // 本次离地后已经使用的跳跃次数
    int maxJumpCount_; // 一次离地最多允许的跳跃次数

    int experience_; //主角经验值
    int level_; //主角等级
    int gold_; //主角金币
    bool isFacingRight_; // 最近一次横向移动方向，初始向右
    bool isMovingHorizontally_; // true 时渲染第二张行走姿势，false 时渲染第一张站立姿势
    float rangedAttackCooldown_; // 下次可发射前的剩余冷却时间
    bool rangedAttackRequested_; // 本帧是否需要由场景生成子弹
    bool isDefending_; // 按住 U 且不在防御冷却时为 true
    float defenseCooldown_; // 成功防御后剩余的不可防御时间，单位：秒
    bool isDodging_; // L 触发后、持续时间未结束时为 true
    bool dodgeRight_; // 本次闪避锁定的方向：true 向右，false 向左
    float dodgeElapsedTime_; // 当前闪避已进行的时间，单位：秒
    float dodgeCooldown_; // 下次可以闪避前的剩余时间，单位：秒
    float lastDodgeDistance_; // 最近一次 update 中闪避实际移动的像素距离

    static constexpr float RANGED_ATTACK_INTERVAL = 0.75F; // 两次远程攻击之间的最短间隔
    static constexpr float DEFENSE_COOLDOWN = 2.0F; // 成功抵挡一次攻击后的冷却
    static constexpr float DODGE_DURATION = 0.2F; // 完成一次闪避所需的时间
    static constexpr float DODGE_COOLDOWN = 5.0F; // 两次闪避之间的冷却

protected:
    // 供 PlayerShadow 在生成时继承玩家的跳跃次数和当前朝向。
    void copyMovementStateFrom(const Player& player);
};
