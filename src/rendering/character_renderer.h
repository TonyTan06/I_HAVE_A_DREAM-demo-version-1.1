#pragma once

#include "raylib.h"

class Enemy;
class Player;
class PlayerShadow;
class PlayerSpriteRenderer;

// GameScene 计算特效是否仍然有效，CharacterRenderer 只读取本帧绘制状态。
struct CharacterEffectView {
    bool playerAttackVisible;
    bool shadowAttackVisible;
    bool meleeEnemyAttackVisible;
    float attackRange;
    float defenseRange;
};

// 统一绘制角色本体、贴身状态条、冷却条和近战刀刃特效。
// 该类不持有纹理或游戏状态，可以安全地由场景长期保存。
class CharacterRenderer {
public:
    void draw(
        const Player& player,
        const Enemy& meleeEnemy,
        const Enemy& rangedEnemy,
        const PlayerShadow* shadow,
        float groundY,
        const PlayerSpriteRenderer& playerSpriteRenderer,
        const CharacterEffectView& effects) const;

private:
    static void drawEnemy(const Enemy& enemy, float groundY, Color eyeColor);
    static void drawBackVerticalCooldownBar(
        const Player& character,
        bool facingRight,
        float characterTop,
        float cooldownProgress);
    static void drawPlayerBlade(
        const Player& character,
        float groundY,
        float attackRange,
        float defenseRange,
        bool attackVisible);
    static void drawMeleeEnemyBlade(
        const Enemy& enemy,
        float groundY,
        float attackRange);
};
