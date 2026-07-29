#include "player.h"

#include <gtest/gtest.h>

TEST(PlayerTest, StartsAtLevelOneWithNoGold) {
    Player player("Johnny");

    EXPECT_EQ(player.getLevel(), 1);
    EXPECT_EQ(player.getGold(), 0);
    EXPECT_EQ(player.getExperience(), 0);
    EXPECT_EQ(player.getMaxJumpCount(), 2);
    EXPECT_FLOAT_EQ(player.getHealth(), 10.0F);
    EXPECT_FLOAT_EQ(player.getMaxHealth(), 10.0F);
    EXPECT_FLOAT_EQ(player.getAttackDamage(), 5.0F);
    EXPECT_EQ(player.getFaction(), Faction::Friendly);
    EXPECT_TRUE(player.isFacingRight());
    EXPECT_FALSE(player.isMovingHorizontally());
    EXPECT_FLOAT_EQ(player.getHitboxWidth(), 96.0F);
    EXPECT_FLOAT_EQ(player.getHitboxHeight(), 128.0F);
}

TEST(PlayerTest, CanRecoverToFullHealthAfterDeath) {
    Player player("Johnny");

    player.takeDamage(player.getMaxHealth());
    player.heal(player.getMaxHealth());

    EXPECT_TRUE(player.isAlive());
    EXPECT_FLOAT_EQ(player.getHealth(), player.getMaxHealth());
}

TEST(PlayerTest, FacesTheDirectionOfItsLastHorizontalMovement) {
    Player player("Johnny");

    player.moveLeft(0.0F);
    EXPECT_FALSE(player.isFacingRight());

    player.moveRight(0.0F);
    EXPECT_TRUE(player.isFacingRight());
}

TEST(PlayerTest, LimitsRangedAttackToOneShotEveryThreeQuartersOfASecond) {
    Player player("Johnny");

    player.rangedAttack();
    EXPECT_TRUE(player.consumeRangedAttackRequest());

    player.rangedAttack();
    EXPECT_FALSE(player.consumeRangedAttackRequest());

    player.update(0.75F);
    player.rangedAttack();
    EXPECT_TRUE(player.consumeRangedAttackRequest());
}

TEST(PlayerTest, BlocksOneAttackThenHasTwoSecondCooldown) {
    Player player("Johnny");
    player.setDefending(true);

    EXPECT_TRUE(player.isDefending());
    EXPECT_TRUE(player.blockNextAttack());
    EXPECT_FALSE(player.isDefending());

    player.setDefending(true);
    EXPECT_FALSE(player.isDefending());
    player.update(2.0F);
    player.setDefending(true);
    EXPECT_TRUE(player.isDefending());
}

TEST(PlayerTest, DefensePreventsRangedAttackRequest) {
    Player player("Johnny");
    player.setDefending(true);
    player.rangedAttack();

    EXPECT_FALSE(player.consumeRangedAttackRequest());
}

TEST(PlayerTest, DodgesOneSecondOfMovementDistanceInTwoTenthsOfASecond) {
    Player player("Johnny");
    const float startingX = player.getX();

    EXPECT_TRUE(player.startDodge(true));
    player.update(0.2F);

    EXPECT_FALSE(player.isDodging());
    EXPECT_FLOAT_EQ(player.getX(), startingX + player.getMoveSpeed());
    EXPECT_FLOAT_EQ(player.getLastDodgeDistance(), player.getMoveSpeed());
    EXPECT_TRUE(player.isDodgeCoolingDown());
    EXPECT_FALSE(player.startDodge(true));

    player.update(4.8F);
    EXPECT_FALSE(player.isDodgeCoolingDown());
    EXPECT_TRUE(player.startDodge(false));
}

TEST(PlayerTest, AddsOnlyPositiveGold) {
    Player player("Johnny");

    player.addGold(25);
    player.addGold(0);
    player.addGold(-5);

    EXPECT_EQ(player.getGold(), 25);
}

TEST(PlayerTest, CannotJumpMoreThanItsMaximumCount) {
    Player player("Johnny");

    player.jump();
    player.jump();
    player.jump();

    EXPECT_EQ(player.getJumpCount(), player.getMaxJumpCount());
}

TEST(PlayerTest, LevelsUpWhenExperienceReachesThreshold) {
    Player player("Johnny");
    const int threshold = player.getExperienceThreshold();
    player.takeDamage(5.0F);

    player.addExperience(threshold);

    EXPECT_EQ(player.getLevel(), 2);
    EXPECT_FLOAT_EQ(player.getMaxHealth(), 15.0F);
    EXPECT_FLOAT_EQ(player.getHealth(), 15.0F);
    EXPECT_FLOAT_EQ(player.getAttackDamage(), 6.0F);
    EXPECT_FLOAT_EQ(player.getAttacksPerSecond(), 0.1F);
}

TEST(PlayerTest, AddsExperienceFromAnEnemyKillReward) {
    Player player("Johnny");
    const int startingExperience = player.getExperience();

    player.addExperience(5);

    EXPECT_EQ(player.getExperience(), startingExperience + 5);
}

TEST(PlayerTest, AppliesDeviceIndependentMovementInput) {
    Player player("Player");
    PlayerInputState input;
    input.moveLeftHeld = true;

    player.applyInput(input, HorizontalInputDirection::Left, 0.5F);

    EXPECT_FLOAT_EQ(player.getX(), 20.0F);
    EXPECT_FALSE(player.isFacingRight());
    EXPECT_TRUE(player.isMovingHorizontally());
}

TEST(PlayerTest, UsesIdlePoseWhenHorizontalInputsCancelEachOther) {
    Player player("Player");
    PlayerInputState input;
    input.moveLeftHeld = true;
    input.moveRightHeld = true;

    player.applyInput(
        input,
        PlayerController::resolveHorizontalDirection(input),
        0.5F);

    EXPECT_FALSE(player.isMovingHorizontally());
    EXPECT_TRUE(player.isFacingRight());
}
