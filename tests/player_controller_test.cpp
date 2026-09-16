#include "input/player_controller.h"

#include <gtest/gtest.h>

TEST(PlayerControllerTest, DefaultInputStateHasNoActiveActions) {
    const PlayerInputState input;
    EXPECT_FALSE(input.moveLeftHeld);
    EXPECT_FALSE(input.moveRightHeld);
    EXPECT_FALSE(input.jumpPressed);
    EXPECT_FALSE(input.meleeAttackPressed);
    EXPECT_FALSE(input.rangedAttackPressed);
    EXPECT_FALSE(input.defendHeld);
    EXPECT_FALSE(input.dodgePressed);
}

TEST(PlayerControllerTest, MergesKeyboardAndGamepadActionsWithOr) {
    PlayerInputState keyboard;
    keyboard.moveLeftHeld = true;
    PlayerInputState gamepad;
    gamepad.jumpPressed = true;

    const PlayerInputState merged =
        PlayerController::mergeInputStates(keyboard, gamepad);
    EXPECT_TRUE(merged.moveLeftHeld);
    EXPECT_TRUE(merged.jumpPressed);
    EXPECT_FALSE(merged.moveRightHeld);
}
