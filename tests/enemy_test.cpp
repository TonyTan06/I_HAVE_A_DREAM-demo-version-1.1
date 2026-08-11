#include "entities/enemy.h"

#include <gtest/gtest.h>

TEST(EnemyTest, StartsAliveAndHasNoLootByDefault) {
    Enemy enemy("Training Dummy");

    EXPECT_TRUE(enemy.isAlive());
    EXPECT_TRUE(enemy.dropLoot().empty());
    EXPECT_FALSE(enemy.isFacingRight());
}

TEST(EnemyTest, FacesTowardTargetsOnEitherSide) {
    Enemy enemy("Enemy");
    Character target("Target");
    enemy.setPosition(300.0F, 0.0F);

    target.setPosition(400.0F, 0.0F);
    enemy.faceToward(target);
    EXPECT_TRUE(enemy.isFacingRight());

    target.setPosition(200.0F, 0.0F);
    enemy.faceToward(target);
    EXPECT_FALSE(enemy.isFacingRight());
}

TEST(EnemyTest, CalculatesExperienceRewardFromEnemyLevel) {
    Enemy levelThreeEnemy("Veteran", 3);

    EXPECT_EQ(levelThreeEnemy.getLevel(), 3);
    EXPECT_EQ(levelThreeEnemy.getExperienceReward(), 15);
}

TEST(EnemyTest, ClampsInvalidLevelToOneForExperienceReward) {
    Enemy invalidLevelEnemy("Invalid", 0);

    EXPECT_EQ(invalidLevelEnemy.getLevel(), 1);
    EXPECT_EQ(invalidLevelEnemy.getExperienceReward(), 5);
}
