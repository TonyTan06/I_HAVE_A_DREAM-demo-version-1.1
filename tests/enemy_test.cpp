#include "test_enemy_fixture.h"

#include <gtest/gtest.h>

TEST(EnemyTest, BaseEnemyCarriesEnemyFactionAndDetectionRange) {
    TestEnemy enemy;
    EXPECT_TRUE(enemy.isAlive());
    EXPECT_EQ(enemy.getFaction(), Faction::Enemy);
    EXPECT_FALSE(enemy.isFacingRight());
    EXPECT_FLOAT_EQ(enemy.getDetectionRange(), 200.0F);
    EXPECT_TRUE(enemy.dropLoot().empty());
}

TEST(EnemyTest, BaseEnemyUsesCharacterAttackCooldowns) {
    TestEnemy enemy;
    EXPECT_TRUE(enemy.tryBeginMeleeAttack());
    EXPECT_FALSE(enemy.tryBeginMeleeAttack());
    enemy.update(1.0F, 980.0F);
    EXPECT_TRUE(enemy.tryBeginMeleeAttack());
}
