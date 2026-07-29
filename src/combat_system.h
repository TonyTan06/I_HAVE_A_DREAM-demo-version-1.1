#pragma once

#include "enemy.h"
#include "melee_enemy.h"
#include "player.h"
#include "raylib.h"

#include <vector>

// CombatSystem 统一负责近战目标选择、攻击框、命中、防御与伤害结算。
// 绘制特效和输入仍属于 Scene，角色数值仍由各 Character 派生类保存。
class CombatSystem {
public:
    struct AttackResult {
        bool attackPerformed; // 是否实际执行了一次攻击；敌人无目标挥刀时也为 true
        bool hit; // 攻击是否命中了可受伤目标
        bool blocked; // 本次命中是否被玩家正前方防御抵挡
        Character* target; // 被命中或成功防御的目标；没有目标时为空
        float damage; // 实际造成的伤害；未命中或被抵挡时为 0
        Rectangle targetHitbox; // 命中目标的屏幕碰撞箱，供伤害数字定位
    };

    CombatSystem(float attackRange, float defenseRange);

    Character* findNearestEnemyTarget(const Enemy& enemy, Player& player) const;
    AttackResult playerMeleeAttack(Player& player,
                                   const std::vector<Enemy*>& enemies,
                                   float platformY) const;
    AttackResult enemyMeleeAttack(MeleeEnemy& enemy, Player& player,
                                  float platformY) const;

    Rectangle makeCharacterHitbox(const Character& character, float platformY) const;
    Rectangle makeMeleeAttackHitbox(const Character& attacker, bool facingRight,
                                    float platformY) const;
    Rectangle makePlayerDefenseHitbox(const Player& player, float platformY) const;
    float getAttackRange() const; // Scene 绘制刀刃时使用的近战长度
    float getDefenseRange() const; // Scene 绘制防御刀刃时使用的防御长度

private:
    float attackRange_; // 近战攻击框向当前朝向延伸的距离
    float defenseRange_; // 玩家防御框向当前朝向延伸的距离
};
