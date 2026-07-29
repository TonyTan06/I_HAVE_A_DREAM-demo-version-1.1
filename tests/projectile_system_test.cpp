#include "systems/projectile_system.h"

#include "entities/melee_enemy.h"
#include "entities/player.h"

#include <gtest/gtest.h>

namespace {
ProjectileSystem::SpawnInfo makePlayerProjectile() {
    return ProjectileSystem::SpawnInfo{
        0.0F, 20.0F, 1.0F, 100.0F, 500.0F, 4.0F, 5.0F,
        Faction::Friendly, ORANGE, 0};
}
} // namespace

TEST(ProjectileSystemTest, DamagesFirstEnemyAndDestroysProjectile) {
    Player player("Player");
    MeleeEnemy enemy(player);
    ProjectileSystem system;
    system.spawn(makePlayerProjectile());

    const Rectangle enemyHitbox{10.0F, 0.0F, 32.0F, 48.0F};
    const auto impacts = system.update(
        0.1F,
        {ProjectileSystem::Target{&enemy, enemyHitbox, false, false, Rectangle{}, {}}});

    ASSERT_EQ(impacts.size(), 1U);
    EXPECT_FLOAT_EQ(enemy.getHealth(), enemy.getMaxHealth() - 5.0F);
    EXPECT_EQ(system.getProjectileCount(), 0U);
}

TEST(ProjectileSystemTest, PassesThroughSameFactionAndInvulnerableTargets) {
    Player player("Player");
    MeleeEnemy enemy(player);
    ProjectileSystem system;
    system.spawn(makePlayerProjectile());

    const Rectangle hitbox{10.0F, 0.0F, 32.0F, 48.0F};
    const auto impacts = system.update(
        0.1F,
        {ProjectileSystem::Target{&player, hitbox, false, false, Rectangle{}, {}},
         ProjectileSystem::Target{&enemy, hitbox, true, false, Rectangle{}, {}}});

    EXPECT_TRUE(impacts.empty());
    EXPECT_FLOAT_EQ(player.getHealth(), player.getMaxHealth());
    EXPECT_FLOAT_EQ(enemy.getHealth(), enemy.getMaxHealth());
    EXPECT_EQ(system.getProjectileCount(), 1U);
}

TEST(ProjectileSystemTest, SuccessfulBlockDestroysProjectileWithoutDamage) {
    Player player("Player");
    ProjectileSystem system;
    auto enemyProjectile = makePlayerProjectile();
    enemyProjectile.faction = Faction::Enemy;
    system.spawn(enemyProjectile);
    player.setDefending(true);

    const Rectangle playerHitbox{10.0F, 0.0F, 32.0F, 48.0F};
    ProjectileSystem::Target target{
        &player, playerHitbox, false, false, Rectangle{}, {}};
    target.hasBlockHitbox = true;
    target.blockHitbox = playerHitbox;
    target.tryBlock = [&player]() { return player.blockNextAttack(); };
    const auto impacts = system.update(0.1F, {target});

    ASSERT_EQ(impacts.size(), 1U);
    EXPECT_TRUE(impacts.front().blocked);
    EXPECT_FLOAT_EQ(player.getHealth(), player.getMaxHealth());
    EXPECT_EQ(system.getProjectileCount(), 0U);
}
