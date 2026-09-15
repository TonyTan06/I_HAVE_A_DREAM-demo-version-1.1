#include "systems/projectile_system.h"
#include "test_enemy_fixture.h"

#include <gtest/gtest.h>

namespace {
ProjectileSystem::SpawnInfo makeProjectile(Faction faction) {
    return ProjectileSystem::SpawnInfo{
        190.0F, 20.0F, 1.0F, 100.0F, 100.0F, 4.0F,
        5.0F, faction, ORANGE, 0};
}
} // namespace

TEST(ProjectileSystemTest, HitUsesCurrentOneDamageCharacterAPI) {
    TestEnemy enemy;
    enemy.setPosition(200.0F, 0.0F);
    ProjectileSystem projectiles;
    projectiles.spawn(makeProjectile(Faction::Friendly));
    const ProjectileSystem::Target target{
        &enemy, Rectangle{200.0F, 0.0F, 20.0F, 40.0F},
        false, false, Rectangle{}, {}};

    const auto impacts = projectiles.update(0.1F, {target});
    ASSERT_EQ(impacts.size(), 1U);
    EXPECT_EQ(enemy.getHealth(), 1);
    EXPECT_FLOAT_EQ(impacts[0].damage, 1.0F);
    EXPECT_EQ(projectiles.getProjectileCount(), 0U);
}

TEST(ProjectileSystemTest, SameFactionTargetIsIgnored) {
    TestEnemy enemy;
    enemy.setPosition(200.0F, 0.0F);
    ProjectileSystem projectiles;
    projectiles.spawn(makeProjectile(Faction::Enemy));
    const ProjectileSystem::Target target{
        &enemy, Rectangle{200.0F, 0.0F, 20.0F, 40.0F},
        false, false, Rectangle{}, {}};

    EXPECT_TRUE(projectiles.update(0.1F, {target}).empty());
    EXPECT_EQ(enemy.getHealth(), 2);
    EXPECT_EQ(projectiles.getProjectileCount(), 1U);
}

TEST(ProjectileSystemTest, ClearRemovesAllProjectiles) {
    ProjectileSystem projectiles;
    projectiles.spawn(makeProjectile(Faction::Friendly));
    projectiles.spawn(makeProjectile(Faction::Enemy));
    ASSERT_EQ(projectiles.getProjectileCount(), 2U);

    projectiles.clear();
    EXPECT_EQ(projectiles.getProjectileCount(), 0U);
}
