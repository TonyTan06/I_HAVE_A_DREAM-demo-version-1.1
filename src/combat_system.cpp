#include "combat_system.h"

#include <cmath>

CombatSystem::CombatSystem(float attackRange, float defenseRange)
    : attackRange_(attackRange),
      defenseRange_(defenseRange) {
}

Character* CombatSystem::findNearestEnemyTarget(
    const Enemy& enemy, Player& player) const {
    Character* nearestTarget = nullptr; // 当前找到的最近有效目标
    float nearestDistance = enemy.getDetectionRange(); // 当前兵种允许的最大检测距离

    const auto considerTarget = [&](Character& candidate, bool invulnerable) {
        if (invulnerable || !candidate.isAlive() ||
            candidate.getFaction() == enemy.getFaction()) return;

        const float horizontalDistance = std::abs(candidate.getX() - enemy.getX());
        if (horizontalDistance <= nearestDistance) {
            nearestDistance = horizontalDistance;
            nearestTarget = &candidate;
        }
    };

    considerTarget(player, player.isDodging());
    return nearestTarget;
}

CombatSystem::AttackResult CombatSystem::playerMeleeAttack(
    Player& player, const std::vector<Enemy*>& enemies,
    float platformY) const {
    const Rectangle attackHitbox =
        makeMeleeAttackHitbox(player, player.isFacingRight(), platformY);
    Character* target = nullptr;
    Rectangle targetHitbox{};

    for (Enemy* enemy : enemies) {
        if (target != nullptr) break;
        if (enemy == nullptr || !enemy->isAlive()) continue;
        const Rectangle enemyHitbox = makeCharacterHitbox(*enemy, platformY);
        if (CheckCollisionRecs(attackHitbox, enemyHitbox)) {
            target = enemy;
            targetHitbox = enemyHitbox;
        }
    }

    if (target == nullptr) {
        return AttackResult{true, false, false, nullptr, 0.0F, Rectangle{}};
    }

    player.attack(*target);
    return AttackResult{
        true, true, false, target, player.getAttackDamage(), targetHitbox};
}

CombatSystem::AttackResult CombatSystem::enemyMeleeAttack(
    MeleeEnemy& enemy, Player& player, float platformY) const {
    // 近战兵无论是否找到目标都会按自身冷却挥刀。
    if (!enemy.tryAttack()) {
        return AttackResult{false, false, false, nullptr, 0.0F, Rectangle{}};
    }

    Character* target = findNearestEnemyTarget(enemy, player);
    if (target == nullptr) {
        return AttackResult{true, false, false, nullptr, 0.0F, Rectangle{}};
    }

    // 锁定目标后先更新朝向，使本次攻击框与刀刃表现保持一致。
    enemy.faceToward(*target);

    const Rectangle attackHitbox =
        makeMeleeAttackHitbox(enemy, enemy.isFacingRight(), platformY);
    const Rectangle targetHitbox = makeCharacterHitbox(*target, platformY);
    if (target == &player && player.isDefending() &&
        CheckCollisionRecs(attackHitbox, makePlayerDefenseHitbox(player, platformY)) &&
        player.blockNextAttack()) {
        return AttackResult{true, false, true, &player, 0.0F, targetHitbox};
    }
    if (!CheckCollisionRecs(attackHitbox, targetHitbox)) {
        return AttackResult{true, false, false, target, 0.0F, targetHitbox};
    }

    target->takeDamage(enemy.getAttackDamage());
    return AttackResult{
        true, true, false, target, enemy.getAttackDamage(), targetHitbox};
}

Rectangle CombatSystem::makeCharacterHitbox(
    const Character& character, float platformY) const {
    return Rectangle{
        character.getX(),
        platformY - character.getHitboxHeight() - character.getY(),
        character.getHitboxWidth(),
        character.getHitboxHeight()};
}

Rectangle CombatSystem::makeMeleeAttackHitbox(
    const Character& attacker, bool facingRight, float platformY) const {
    const float attackX = facingRight
        ? attacker.getX() + attacker.getHitboxWidth()
        : attacker.getX() - attackRange_;
    return Rectangle{
        attackX,
        platformY - attacker.getHitboxHeight() - attacker.getY(),
        attackRange_,
        attacker.getHitboxHeight()};
}

Rectangle CombatSystem::makePlayerDefenseHitbox(
    const Player& player, float platformY) const {
    const float defenseX = player.isFacingRight()
        ? player.getX() + player.getHitboxWidth()
        : player.getX() - defenseRange_;
    return Rectangle{
        defenseX,
        platformY - player.getHitboxHeight() - player.getY(),
        defenseRange_,
        player.getHitboxHeight()};
}

float CombatSystem::getAttackRange() const {
    return attackRange_;
}

float CombatSystem::getDefenseRange() const {
    return defenseRange_;
}
