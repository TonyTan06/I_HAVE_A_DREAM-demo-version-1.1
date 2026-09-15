#include "entities/player.h"
#include "systems/combat_system.h"
#include "test_enemy_fixture.h"

#include <gtest/gtest.h>

TEST(CombatSystemTest, PlayerMeleeUsesCurrentHitAndCooldownAPI) {
    Player player("Player");
    TestEnemy enemy;
    enemy.setPosition(236.0F, 0.0F);
    CombatSystem combat;

    const auto first = combat.playerMeleeAttack(player, {&enemy}, 360.0F);
    EXPECT_TRUE(first.attackPerformed);
    EXPECT_TRUE(first.hit);
    EXPECT_EQ(enemy.getHealth(), 1);
    EXPECT_FLOAT_EQ(first.damage, 1.0F);

    const auto second = combat.playerMeleeAttack(player, {&enemy}, 360.0F);
    EXPECT_FALSE(second.attackPerformed);
    EXPECT_EQ(enemy.getHealth(), 1);
}

TEST(CombatSystemTest, EnemyDetectionHonorsConfiguredRange) {
    Player player("Player");
    TestEnemy enemy;
    enemy.setPosition(100.0F, 0.0F);
    enemy.setDetectionRangeForTest(40.0F);
    CombatSystem combat;

    player.setPosition(141.0F, 0.0F);
    EXPECT_EQ(combat.findNearestEnemyTarget(enemy, player), nullptr);
    player.setPosition(140.0F, 0.0F);
    EXPECT_EQ(combat.findNearestEnemyTarget(enemy, player), &player);
}

TEST(CombatSystemTest, EnemyMeleeCanBeBlocked) {
    Player player("Player");
    player.setDefending(true);
    TestEnemy enemy;
    enemy.setPosition(252.0F, 0.0F);
    CombatSystem combat;

    const auto result = combat.enemyMeleeAttack(enemy, player, 360.0F);
    EXPECT_TRUE(result.attackPerformed);
    EXPECT_TRUE(result.blocked);
    EXPECT_EQ(player.getHealth(), 1);
}

TEST(CombatSystemTest, EnemyRangedRequiresCapabilityAndCooldown) {
    Player player("Player");
    TestEnemy enemy;
    enemy.setPosition(100.0F, 0.0F);
    CombatSystem combat;

    EXPECT_TRUE(combat.tryEnemyRangedAttack(enemy, player).projectileRequested);
    EXPECT_FALSE(combat.tryEnemyRangedAttack(enemy, player).projectileRequested);
    enemy.update(1.0F, 980.0F);
    enemy.setRangedRangeForTest(0.0F);
    EXPECT_FALSE(combat.tryEnemyRangedAttack(enemy, player).projectileRequested);
}

TEST(CombatSystemTest, HitboxesUseCharacterRangesWithoutSystemLimits) {
    Player player("Player");
    CombatSystem combat;

    const Rectangle melee =
        combat.makeMeleeAttackHitbox(player, true, 360.0F);
    const Rectangle defense =
        combat.makePlayerDefenseHitbox(player, 360.0F);
    EXPECT_FLOAT_EQ(melee.width, player.getMeleeAttackRange());
    EXPECT_FLOAT_EQ(defense.width, player.getDefenseRange());
    EXPECT_FLOAT_EQ(melee.width, 48.0F);
    EXPECT_FLOAT_EQ(defense.width, 16.0F);
}
