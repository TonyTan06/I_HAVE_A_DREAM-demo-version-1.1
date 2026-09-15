#pragma once

#include "entities/enemy.h"
#include "entities/player.h"
#include "raylib.h"

#include <vector>

// CombatSystem 统一负责敌军索敌、攻击机会及近战命中、防御与伤害结算。
// 攻击范围由角色属性提供；GameWorld 协调弹道生成，GameScene 负责视觉反馈。
class CombatSystem {
public:
    struct AttackResult {
        bool attackPerformed; // 是否实际执行了一次攻击；敌人无目标挥刀时也为 true
        bool hit; // 攻击是否命中了可受伤目标
        bool blocked; // 本次命中是否被玩家正前方防御抵挡
        Character* target; // 被命中或成功防御的目标；没有目标时为空
        float damage; // 当前无数值伤害 API；命中时用于 HUD 的单次受击数值
        Rectangle targetHitbox; // 命中目标的屏幕碰撞箱，供伤害数字定位
    };

    struct RangedAttackResult {
        bool projectileRequested; // 是否成功消耗攻击冷却并应生成一枚弹道
        const Character* target; // 本帧只读索敌结果；范围内没有有效目标时为空
    };

    CombatSystem() = default;

    Character* findNearestEnemyTarget(const Enemy& enemy, Player& player) const;
    AttackResult playerMeleeAttack(Player& player,
                                   const std::vector<Enemy*>& enemies,
                                   float platformY) const;
    AttackResult enemyMeleeAttack(Enemy& enemy, Player& player,
                                  float platformY) const;
    // 完成敌人的远程范围与攻击冷却判定；弹道参数仍由上层配置。
    RangedAttackResult tryEnemyRangedAttack(
        Enemy& enemy, Player& player) const;

    Rectangle makeCharacterHitbox(const Character& character, float platformY) const;
    Rectangle makeMeleeAttackHitbox(const Character& attacker, bool facingRight,
                                    float platformY) const;
    Rectangle makePlayerDefenseHitbox(const Player& player, float platformY) const;

private:
    static void considerTarget(
        const Enemy& enemy,
        Character& candidate,
        bool invulnerable,
        Character*& nearestTarget,
        float& nearestDistance);
};
