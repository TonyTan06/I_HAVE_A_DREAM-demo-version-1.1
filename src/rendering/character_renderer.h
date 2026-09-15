#pragma once

#include "raylib.h"

class Enemy;
class Player;
class PlayerShadow;
class PlayerSpriteRenderer;

// 统一绘制角色本体、贴身状态条、冷却条和近战刀刃特效。
// 该类不持有纹理或游戏状态，可以安全地由场景长期保存。
class CharacterRenderer {
public:
    void drawPlayer(
        const Player& player,
        float groundY,
        const PlayerSpriteRenderer& playerSpriteRenderer,
        bool attackVisible,
        float attackRange,
        float defenseRange) const;
    // 影子本体和时间条由 ShadowManager 绘制，这里叠加近战刀刃。
    void drawShadow(
        const PlayerShadow& shadow,
        float groundY,
        bool attackVisible,
        float attackRange,
        float defenseRange) const;
    void drawEnemy(
        const Enemy& enemy,
        float groundY,
        Color eyeColor,
        bool attackVisible,
        float attackRange) const;

private:
    static void drawBackVerticalCooldownBar(
        const Player& character,
        bool facingRight,
        float characterTop,
        float cooldownProgress
    );

    static void drawPlayerBlade(
        const Player& character,
        float groundY,
        float attackRange,
        float defenseRange,
        bool attackVisible
    );

    static void drawEnemyBlade(
        const Enemy& enemy,
        float groundY,
        float attackRange
    );
};
