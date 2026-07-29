#include "entities/melee_enemy.h"

#include "entities/player.h"

MeleeEnemy::MeleeEnemy(const Player& player, int level)
    : Enemy("MELEE_ENEMY", level) {
    moveSpeed_ = player.getMoveSpeed();
    physicalDefense_ = player.getPhysicalDefense();
    magicalDefense_ = player.getMagicalDefense();
    health_ = player.getHealth();
    maxHealth_ = player.getMaxHealth();
    attackDamage_ = player.getAttackDamage();
    attacksPerSecond_ = 2.0F;
    shield_ = player.getShield();
    jumpInitialVelocity_ = player.getJumpInitialVelocity();
    gravityScale_ = player.getGravityScale();
    detectionRange_ = 150.0F;
    attackCooldown_ = 0.5F;
}
