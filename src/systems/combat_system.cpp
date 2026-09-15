#include "systems/combat_system.h"

#include <algorithm>
#include <cmath>

Character* CombatSystem::findNearestEnemyTarget(
    const Enemy& enemy, Player& player) const {
    Character* nearestTarget = nullptr; // 当前找到的最近有效目标
    float nearestDistance = enemy.getDetectionRange(); // 当前兵种允许的最大检测距离

    considerTarget(
        enemy,
        player,
        player.isDodging(),
        nearestTarget,
        nearestDistance);
    return nearestTarget;
}

void CombatSystem::considerTarget(
    const Enemy& enemy,
    Character& candidate,
    bool invulnerable,
    Character*& nearestTarget,
    float& nearestDistance) {
    if (invulnerable || !candidate.isAlive() ||
        candidate.getFaction() == enemy.getFaction()) {
        return;
    }

    const float horizontalDistance =
        std::abs(candidate.getX() - enemy.getX());
    if (horizontalDistance <= nearestDistance) {
        nearestDistance = horizontalDistance;
        nearestTarget = &candidate;
    }
}

CombatSystem::AttackResult CombatSystem::playerMeleeAttack(
    Player& player, const std::vector<Enemy*>& enemies,
    float platformY) const {
    if (player.getMeleeAttackRange() <= 0.0F ||
        !player.tryBeginMeleeAttack()) {
        return AttackResult{false, false, false, nullptr, 0.0F, Rectangle{}};
    }
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

    player.applyMeleeHit(*target);
    return AttackResult{
        true, true, false, target, 1.0F, targetHitbox};
}

CombatSystem::AttackResult CombatSystem::enemyMeleeAttack(
    Enemy& enemy, Player& player, float platformY) const {
    if (enemy.getMeleeAttackRange() <= 0.0F ||
        !enemy.tryBeginMeleeAttack()) {
        return AttackResult{false, false, false, nullptr, 0.0F, Rectangle{}};
    }

    Character* target = findNearestEnemyTarget(enemy, player);
    if (target == nullptr) {
        return AttackResult{true, false, false, nullptr, 0.0F, Rectangle{}};
    }

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

    target->takeDamage();
    return AttackResult{
        true, true, false, target, 1.0F, targetHitbox};
}

CombatSystem::RangedAttackResult CombatSystem::tryEnemyRangedAttack(
    Enemy& enemy, Player& player) const {
    if (enemy.getRangedAttackRange() <= 0.0F) {
        return RangedAttackResult{false, nullptr};
    }
    Character* target = findNearestEnemyTarget(enemy, player);
    if (target == nullptr ||
        std::abs(target->getX() - enemy.getX()) >
            enemy.getRangedAttackRange()) {
        return RangedAttackResult{false, nullptr};
    }

    return RangedAttackResult{enemy.tryBeginRangedAttack(), target};
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
    const float effectiveRange = attacker.getMeleeAttackRange();
    const float attackX = facingRight ? attacker.getX() + attacker.getHitboxWidth() : attacker.getX() - effectiveRange;
    return Rectangle{
        attackX,
        platformY - attacker.getHitboxHeight() - attacker.getY(),
        effectiveRange,
        attacker.getHitboxHeight()};
}

Rectangle CombatSystem::makePlayerDefenseHitbox(
    const Player& player, float platformY) const {
    const float defenseRange = player.getDefenseRange();
    const float defenseX = player.isFacingRight() ? player.getX() + player.getHitboxWidth() : player.getX() - defenseRange;
    return Rectangle{
        defenseX,
        platformY - player.getHitboxHeight() - player.getY(),
        defenseRange,
        player.getHitboxHeight()};
}
