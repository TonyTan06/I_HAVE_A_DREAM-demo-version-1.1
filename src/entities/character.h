#pragma once

#include <string>

enum class Faction {
    Friendly, // 玩家所属阵营
    Enemy,    // 敌人所属阵营
    Neutral   // 影子等可被任意阵营攻击的第三方单位
};

// 所有可移动、受重力影响的游戏实体共用的基础类型。
class Character {
public:
    explicit Character(std::string name);//创建人物要填写名字

    //deltaTime 是上一帧到当前帧经过了多少秒。
    void moveRight(float deltaTime); //右移
    void moveLeft(float deltaTime); //左移
    void jump(); //跳跃
    virtual void takeDamage(); //受击伤害；无血量实体可以覆写为空操作
    virtual void takeTrueDamage(); //受到真实伤害
    virtual void update(float deltaTime, float worldGravity); //每一帧更新角色函数
    void setPosition(float x, float y); //设置角色位置
    void land(); //将角色放回地面并重置竖直速度
    void landAtHeight(float height); //让角色站在指定离地高度并重置竖直速度
    void beginFalling(); //角色走出高处平台时解除落地状态并开始受重力影响
    void setFaction(Faction faction); // 设置角色所属阵营
    void setGravityScale(float gravityScale); //设置角色自身重力倍率，1.0F 就是 1g
    void stopMovingHorizontally(); // 停止普通水平移动并回到站立姿势

    virtual bool tryBeginMeleeAttack(); // 冷却完成时开始一次近战攻击并重新计时
    virtual bool tryBeginRangedAttack(); // 冷却完成时开始一次远程攻击并重新计时
    bool isAlive() const; //检测存活
    bool isGrounded() const; // 是否站在平台或地面上
    bool isFacingRight() const; // 最近一次横向移动方向，初始向右
    bool isMovingHorizontally() const; // true 时渲染第二张行走姿势，false 时渲染第一张站立姿势
    bool isMeleeAttackReady() const; // 近战攻击剩余冷却是否已经归零
    bool isRangedAttackReady() const; // 远程攻击剩余冷却是否已经归零

    float getX() const; //获取角色 x 轴位置
    float getY() const; //获取角色相对地面的高度
    float getHitboxWidth() const; // 当前角色受击箱和平台碰撞箱的宽度，单位：像素
    float getHitboxHeight() const; // 当前角色受击箱和平台碰撞箱的高度，单位：像素
    float getMoveSpeed() const;
    float getMeleeAttackInterval() const;
    float getRangedAttackInterval() const;
    float getJumpInitialVelocity() const;
    float getGravityScale() const; //获取角色自身重力倍率
    float getMeleeAttackRange() const; // 近战攻击的有效范围，单位：像素
    float getRangedAttackRange() const; // 远程攻击的有效范围，单位：像素

    int getHealth() const; // 当前生命值，用于状态条和受击判定
    int getShield() const;
    int getJumpCount() const; //跳跃次数（可以实现多段跳）
    int getMaxJumpCount() const; //最大跳跃次数

    Faction getFaction() const; // 获取阵营，供战斗目标筛选使用

    virtual ~Character() = default;

protected:
    std::string name_; //角色名字

    bool isGrounded_; //检测角色是否下落到实体上
    bool isFacingRight_; // 最近一次横向移动方向，初始向右
    bool isMovingHorizontally_; // true 时渲染第二张行走姿势，false 时渲染第一张站立姿势

    float x_; //角色x轴位置信息
    float y_; //角色y轴位置信息
    float moveSpeed_; //角色移动速度
    float meleeAttackInterval_; // 两次近战攻击之间的最短间隔，单位：秒
    float rangedAttackInterval_; // 两次远程攻击之间的最短间隔，单位：秒
    float meleeAttackCooldownRemaining_; // 下次可开始近战攻击前的剩余时间
    float rangedAttackCooldownRemaining_; // 下次可开始远程攻击前的剩余时间
    float hitboxWidth_; // 角色自身的碰撞箱宽度；允许玩家和敌人使用不同尺寸
    float hitboxHeight_; // 角色自身的碰撞箱高度；玩家当前尺寸来自素材单帧大小
    float jumpInitialVelocity_; //角色跳跃初速度
    float verticalVelocity_; //角色当前的垂直速度
    float gravityScale_; //角色受到的重力倍率，1.0F 是 1g，0.5F 是半重力，2.0F 是双倍重力
    //不同角色可以设置不同的重力倍率，影响跳跃高度和下落速度
    float meleeAttackRange_; // 近战攻击的有效范围，单位：像素
    float rangedAttackRange_; // 远程攻击的有效范围，单位：像素

    int health_; //角色当前血量
    int shield_; //角色护盾值
    int jumpCount_; // 本次离地后已经使用的跳跃次数
    int maxJumpCount_; // 一次离地最多允许的跳跃次数

    Faction faction_;

    // 派生角色在构造时按自身素材设置碰撞箱；无效尺寸不会覆盖原值。
    void setHitboxSize(float width, float height);
};
