#include "entities/player.h"
#include "entities/ranged_enemy.h"

#include <gtest/gtest.h>

TEST(RangedEnemyTest, CopiesPlayerCombatStatsAndFacesLeft) {
    Player player("Player");
    RangedEnemy enemy(player);

    EXPECT_FLOAT_EQ(enemy.getHealth(), player.getHealth());
    EXPECT_FLOAT_EQ(enemy.getMaxHealth(), player.getMaxHealth());
    EXPECT_FLOAT_EQ(enemy.getAttackDamage(), player.getAttackDamage());
    EXPECT_FLOAT_EQ(enemy.getAttacksPerSecond(), 1.25F);
    EXPECT_FLOAT_EQ(enemy.getDetectionRange(), 300.0F);
    EXPECT_FALSE(enemy.isFacingRight());
    EXPECT_EQ(enemy.getFaction(), Faction::Enemy);
}

TEST(RangedEnemyTest, UsesItsLevelToCalculateExperienceReward) {
    Player player("Player");
    RangedEnemy enemy(player, 4);

    EXPECT_EQ(enemy.getLevel(), 4);
    EXPECT_EQ(enemy.getExperienceReward(), 20);
}

TEST(RangedEnemyTest, UsesItsConfiguredAttackCooldown) {
    Player player("Player");
    RangedEnemy enemy(player);

    enemy.update(0.8F);

    EXPECT_TRUE(enemy.tryAttack());
    EXPECT_FALSE(enemy.tryAttack());
}

TEST(RangedEnemyTest, FacesTowardTargetsOnEitherSide) {
    Player player("Player");
    RangedEnemy enemy(player);
    Player target("Target");
    enemy.setPosition(300.0F, 0.0F);

    target.setPosition(400.0F, 0.0F);
    enemy.faceToward(target);
    EXPECT_TRUE(enemy.isFacingRight());

    target.setPosition(200.0F, 0.0F);
    enemy.faceToward(target);
    EXPECT_FALSE(enemy.isFacingRight());
}
