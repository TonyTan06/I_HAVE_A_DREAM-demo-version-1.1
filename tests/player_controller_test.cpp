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
    EXPECT_FALSE(input.selectShadowSkill1Pressed);
    EXPECT_FALSE(input.useShadowSkill2Pressed);
}

TEST(PlayerControllerTest, MergesInputFromDifferentDevices) {
    PlayerInputState keyboardInput;
    keyboardInput.moveLeftHeld = true;
    keyboardInput.meleeAttackPressed = true;

    PlayerInputState gamepadInput;
    gamepadInput.moveRightHeld = true;
    gamepadInput.dodgePressed = true;
    gamepadInput.selectShadowSkill1Pressed = true;
    gamepadInput.useShadowSkill2Pressed = true;

    const PlayerInputState merged =
        PlayerController::mergeInputStates(keyboardInput, gamepadInput);

    EXPECT_TRUE(merged.moveLeftHeld);
    EXPECT_TRUE(merged.moveRightHeld);
    EXPECT_TRUE(merged.meleeAttackPressed);
    EXPECT_TRUE(merged.dodgePressed);
    EXPECT_TRUE(merged.selectShadowSkill1Pressed);
    EXPECT_TRUE(merged.useShadowSkill2Pressed);
    EXPECT_FALSE(merged.jumpPressed);
    EXPECT_FALSE(merged.rangedAttackPressed);
    EXPECT_FALSE(merged.defendHeld);
}

TEST(PlayerControllerTest, ResolvesSingleHorizontalDirection) {
    PlayerInputState input;
    input.moveLeftHeld = true;

    EXPECT_EQ(PlayerController::resolveHorizontalDirection(input),
              HorizontalInputDirection::Left);

    input.moveLeftHeld = false;
    input.moveRightHeld = true;
    EXPECT_EQ(PlayerController::resolveHorizontalDirection(input),
              HorizontalInputDirection::Right);
}

TEST(PlayerControllerTest, CancelsConflictingHorizontalInput) {
    PlayerInputState input;
    input.moveLeftHeld = true;
    input.moveRightHeld = true;

    EXPECT_EQ(PlayerController::resolveHorizontalDirection(input),
              HorizontalInputDirection::None);
}
