#include "entities/melee_enemy.h"
#include "entities/player.h"

#include <gtest/gtest.h>

TEST(MeleeEnemyTest, CopiesPlayerCombatStatsAndFacesLeft) {
    Player player("Player");
    MeleeEnemy enemy(player);

    EXPECT_FLOAT_EQ(enemy.getHealth(), player.getHealth());
    EXPECT_FLOAT_EQ(enemy.getMaxHealth(), player.getMaxHealth());
    EXPECT_FLOAT_EQ(enemy.getAttackDamage(), player.getAttackDamage());
    EXPECT_FLOAT_EQ(enemy.getAttacksPerSecond(), 2.0F);
    EXPECT_FLOAT_EQ(enemy.getDetectionRange(), 150.0F);
    EXPECT_FALSE(enemy.isFacingRight());
    EXPECT_EQ(enemy.getFaction(), Faction::Enemy);
}

TEST(MeleeEnemyTest, FacesTowardTargetsOnEitherSide) {
    Player player("Player");
    MeleeEnemy enemy(player);
    Player target("Target");
    enemy.setPosition(300.0F, 0.0F);

    target.setPosition(400.0F, 0.0F);
    enemy.faceToward(target);
    EXPECT_TRUE(enemy.isFacingRight());

    target.setPosition(200.0F, 0.0F);
    enemy.faceToward(target);
    EXPECT_FALSE(enemy.isFacingRight());
}

TEST(MeleeEnemyTest, UsesItsLevelToCalculateExperienceReward) {
    Player player("Player");
    MeleeEnemy enemy(player, 2);

    EXPECT_EQ(enemy.getLevel(), 2);
    EXPECT_EQ(enemy.getExperienceReward(), 10);
}

TEST(MeleeEnemyTest, DoesNotDamageAnEnemyFactionTarget) {
    Player player("Player");
    MeleeEnemy attacker(player);
    MeleeEnemy ally(player);

    attacker.update(0.5F);

    EXPECT_FALSE(attacker.attack(ally));
    EXPECT_FLOAT_EQ(ally.getHealth(), ally.getMaxHealth());
}

TEST(MeleeEnemyTest, RespawnsAtDeathPositionAfterFiveSeconds) {
    Player player("Player");
    MeleeEnemy enemy(player);
    enemy.setPosition(200.0F, 0.0F);
    enemy.takeDamage(enemy.getMaxHealth());

    enemy.update(4.0F);
    EXPECT_FALSE(enemy.isAlive());
    EXPECT_TRUE(enemy.isRespawning());
    EXPECT_FLOAT_EQ(enemy.getRespawnProgress(), 0.8F);

    enemy.update(1.0F);
    EXPECT_TRUE(enemy.isAlive());
    EXPECT_FALSE(enemy.isRespawning());
    EXPECT_FLOAT_EQ(enemy.getHealth(), enemy.getMaxHealth());
    EXPECT_FLOAT_EQ(enemy.getX(), 200.0F);
}
