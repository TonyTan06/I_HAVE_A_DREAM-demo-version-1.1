#pragma once

#include "entities/character.h"
#include "raylib.h"

#include <cstddef>
#include <functional>
#include <vector>

// ProjectileSystem 统一负责弹道的生命周期、碰撞、伤害、穿透计数和绘制。
// Scene 只需要提交发射参数与本帧可受击目标，不再直接遍历或删除子弹。
class ProjectileSystem {
public:
    struct SpawnInfo {
        float x; // 新弹道圆心的初始屏幕 x 坐标
        float y; // 新弹道圆心的初始屏幕 y 坐标
        float direction; // 水平飞行方向：正数向右，负数向左
        float speed; // 弹道飞行速度，单位：像素/秒
        float maxDistance; // 未命中时允许飞行的最大距离，单位：像素
        float radius; // 圆形碰撞体与绘制使用的半径
        float damage; // 每次有效命中造成的生命伤害
        Faction faction; // 发射者阵营，同阵营目标会被直接忽略
        Color color; // 弹道绘制颜色
        int extraPenetrations = 0; // 击中首个目标后还能额外穿透的目标数量
    };

    struct Target {
        Character* character; // 接收伤害的角色实体；空指针目标会被忽略
        Rectangle hitbox; // 角色当前帧的屏幕碰撞箱
        bool invulnerable = false; // true 时弹道穿过目标，且不触发伤害和销毁
        bool hasBlockHitbox = false; // 是否启用独立于身体碰撞箱的拦截区域
        Rectangle blockHitbox{}; // 盾牌、防御刀刃等用于提前拦截弹道的区域
        std::function<bool()> tryBlock; // 拦截区域命中后调用；返回 true 表示成功抵挡
    };

    struct Impact {
        Character* target; // 本次实际受伤的角色；被成功抵挡时为空
        float damage; // 本次结算的伤害；被抵挡时为 0
        float textX; // Scene 显示伤害数字使用的 x 坐标
        float textY; // Scene 显示伤害数字使用的 y 坐标
        bool blocked; // true 表示弹道被防御机制拦截而非造成伤害
    };

    void spawn(const SpawnInfo& spawnInfo); // 按给定参数创建并保存一枚新弹道
    std::vector<Impact> update(float deltaTime, const std::vector<Target>& targets);
    void draw() const; // 绘制所有仍处于飞行状态的弹道
    std::size_t getProjectileCount() const; // 返回当前存活弹道数量，供测试与调试使用

private:
    struct Projectile {
        SpawnInfo properties; // 该弹道生成时固定的速度、伤害、颜色等配置
        float x; // 当前圆心屏幕 x 坐标
        float y; // 当前圆心屏幕 y 坐标
        float travelledDistance; // 当前已经累计飞行的距离
        int remainingPenetrations; // 当前还能额外穿透多少个有效目标
        std::vector<const Character*> hitTargets; // 已命中过的目标，防止穿透时重复伤害
    };

    static bool hasHitTarget(const Projectile& projectile, const Character* target);

    std::vector<Projectile> projectiles_; // 系统当前管理的全部飞行中弹道
};
