#include "entities/player.h"
#include "systems/player_action_system.h"

#include <gtest/gtest.h>

TEST(PlayerActionSystemTest, MovesFromDeviceIndependentInput) {
    Player player("Player");
    PlayerActionSystem actions;
    PlayerInputState input;
    input.moveLeftHeld = true;

    actions.applyActions(player, input, 0.5F);
    EXPECT_FLOAT_EQ(player.getX(), 20.0F);
    EXPECT_FALSE(player.isFacingRight());
}

TEST(PlayerActionSystemTest, OppositeDirectionsKeepIdleFacing) {
    Player player("Player");
    PlayerActionSystem actions;
    PlayerInputState input;
    input.moveLeftHeld = true;
    input.moveRightHeld = true;

    actions.applyActions(player, input, 0.5F);
    EXPECT_FLOAT_EQ(player.getX(), 140.0F);
    EXPECT_TRUE(player.isFacingRight());
    EXPECT_FALSE(player.isMovingHorizontally());
}

TEST(PlayerActionSystemTest, DefenseSuppressesAttackRequests) {
    Player player("Player");
    PlayerActionSystem actions;
    PlayerInputState input;
    input.defendHeld = true;
    input.meleeAttackPressed = true;
    input.rangedAttackPressed = true;

    const auto result = actions.applyActions(player, input, 0.0F);
    EXPECT_TRUE(player.isDefending());
    EXPECT_FALSE(result.playerMeleeAttackRequested);
    EXPECT_FALSE(player.consumeRangedAttackRequest());
}

TEST(PlayerActionSystemTest, ReportsMeleeAndQueuesRangedSeparately) {
    Player player("Player");
    PlayerActionSystem actions;
    PlayerInputState input;
    input.meleeAttackPressed = true;
    input.rangedAttackPressed = true;

    const auto result = actions.applyActions(player, input, 0.0F);
    EXPECT_TRUE(result.playerMeleeAttackRequested);
    EXPECT_TRUE(player.consumeRangedAttackRequest());
}

TEST(PlayerActionSystemTest, DodgeUsesFacingWhenNoDirectionHeld) {
    Player player("Player");
    PlayerActionSystem actions;
    PlayerInputState input;
    input.dodgePressed = true;

    actions.applyActions(player, input, 0.0F);
    EXPECT_TRUE(player.isDodging());
    player.update(0.2F, 980.0F);
    EXPECT_FLOAT_EQ(player.getX(), 380.0F);
}
