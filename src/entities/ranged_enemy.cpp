#include "entities/ranged_enemy.h"

#include "entities/player.h"

RangedEnemy::RangedEnemy(const Player& player, int level)
    : Enemy("RANGED_ENEMY", level) {
    moveSpeed_ = player.getMoveSpeed();
    physicalDefense_ = player.getPhysicalDefense();
    magicalDefense_ = player.getMagicalDefense();
    health_ = player.getHealth();
    maxHealth_ = player.getMaxHealth();
    attackDamage_ = player.getAttackDamage();
    attacksPerSecond_ = 1.25F;
    shield_ = player.getShield();
    jumpInitialVelocity_ = player.getJumpInitialVelocity();
    gravityScale_ = player.getGravityScale();
    detectionRange_ = 300.0F;
    attackCooldown_ = 0.8F;
}
