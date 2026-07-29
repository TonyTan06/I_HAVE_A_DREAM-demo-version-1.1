#pragma once

#include <string>

enum class Faction {
    Friendly, // 玩家所属阵营
    Enemy,    // 敌人所属阵营
    Neutral   // 影子等可被任意阵营攻击的第三方单位
};

class Character {
public:
    static constexpr float BASE_GRAVITY = 980.0F; //基础重力加速度，单位大约是像素/秒²。可根据比例尺更改

    explicit Character(std::string name);//创建人物要填写名字

    void moveRight(float deltaTime); //右移
    void moveLeft(float deltaTime); //左移
    //deltaTime 是上一帧到当前帧经过了多少秒。
    virtual void jump(); //跳跃
    virtual void takeDamage(float damage); //受击伤害；无血量实体可以覆写为空操作
    void heal(float value); //回血量
    virtual void attack(); //通用攻击代码
    // 远程攻击框架。具体投射物、伤害、冷却和特效由派生角色自行实现。
    virtual void rangedAttack();
    virtual void update(float deltaTime); //每一帧更新角色函数
    
    bool isAlive() const; //检测存活
    bool isGrounded() const; // 是否站在平台或地面上
    float getHealth() const; // 当前生命值，用于状态条和受击判定
    float getX() const; //获取角色 x 轴位置
    float getY() const; //获取角色相对地面的高度
    float getHitboxWidth() const; // 当前角色受击箱和平台碰撞箱的宽度，单位：像素
    float getHitboxHeight() const; // 当前角色受击箱和平台碰撞箱的高度，单位：像素
    void setPosition(float x, float y); //设置角色位置
    void land(); //将角色放回地面并重置竖直速度
    void landAtHeight(float height); //让角色站在指定离地高度并重置竖直速度
    void beginFalling(); //角色走出高处平台时解除落地状态并开始受重力影响
    void swapPositionAndPhysics(Character& other); //交换坐标、竖直速度和落地状态
    float getMoveSpeed() const;
    float getPhysicalDefense() const;
    float getMagicalDefense() const;
    float getMaxHealth() const;
    float getAttackDamage() const;
    float getAttacksPerSecond() const;
    float getShield() const;
    float getJumpInitialVelocity() const;
    Faction getFaction() const; // 获取阵营，供战斗目标筛选使用
    void setFaction(Faction faction); // 设置角色所属阵营
    float getGravityScale() const; //获取角色自身重力倍率
    void setGravityScale(float gravityScale); //设置角色自身重力倍率，1.0F 就是 1g

    virtual ~Character() = default;

protected:
    std::string name_; //角色名字

    float x_; //角色x轴位置信息
    float y_; //角色y轴位置信息
    float moveSpeed_; //角色移动速度
    float physicalDefense_; //角色物理防御
    float magicalDefense_; //角色魔法防御
    float health_; //角色当前血量
    float maxHealth_; //角色最大血量
    float attackDamage_; //角色攻击伤害
    float attacksPerSecond_; //角色每秒攻击速度
    float shield_; //角色护盾值
    float hitboxWidth_; // 角色自身的碰撞箱宽度；允许玩家和敌人使用不同尺寸
    float hitboxHeight_; // 角色自身的碰撞箱高度；玩家当前尺寸来自素材单帧大小

    //不同角色可以设置不同的重力倍率，影响跳跃高度和下落速度
    float jumpInitialVelocity_; //角色跳跃初速度
    float verticalVelocity_; //角色当前的垂直速度
    float gravityScale_; //角色受到的重力倍率，1.0F 是 1g，0.5F 是半重力，2.0F 是双倍重力

    bool isGrounded_; //检测角色是否下落到实体上
    Faction faction_;

    // 派生角色在构造时按自身素材设置碰撞箱；无效尺寸不会覆盖原值。
    void setHitboxSize(float width, float height);
};
