#pragma once

#include "player_shadow.h"
#include "raylib.h"

#include <optional>

class Player;
class PlayerSpriteRenderer;

// ShadowManager 只负责影子的记录点、生成距离、实体生命周期和销毁。
// 影子的技能选择与执行全部属于 PlayerShadow，不在管理器中处理。
class ShadowManager {
public:
    explicit ShadowManager(const Player& player); // 以玩家出生位置建立第一个记录点

    void update(const Player& player, float deltaTime); // 更新距离、生成、重力和持续时间
    void resetPlayerTracking(const Player& player); // 玩家瞬移后重置距离跟踪
    // 使用玩家同一套双帧素材绘制记录点和已生成影子。
    void draw(float platformY, const PlayerSpriteRenderer& spriteRenderer) const;

    bool hasShadow() const; // 当前是否存在影子实体
    PlayerShadow* getShadow(); // 返回当前影子；不存在时返回 nullptr
    const PlayerShadow* getShadow() const; // 只读版本的影子访问接口

private:
    std::optional<PlayerShadow> shadow_; // 同时最多存在一个影子实体
    float recordedPlayerX_; // 本轮影子生成记录点的 x 坐标
    float recordedPlayerY_; // 本轮影子生成记录点的离地高度
    float previousPlayerX_; // 上一帧玩家 x 坐标，用于累计水平移动
    bool recordedFacingRight_; // 记录点模型生成时玩家的朝向
    float accumulatedHorizontalDistance_; // 本轮累计的实际水平移动距离
    float elapsedLifetime_; // 当前影子已经存在的时间，单位：秒

    static constexpr float SPAWN_DISTANCE = 300.0F; // 生成影子所需累计距离
    static constexpr float LIFETIME = 10.0F; // 影子存在时间上限
    static constexpr float DODGE_DISTANCE_MULTIPLIER = 0.33F; // 240px 闪避只累计 79.2px
};
