#include "input/player_controller.h"

#include "raylib.h"

PlayerInputState PlayerController::pollInput() const {
    // 后续接入手柄时，在此读取手柄状态并通过 mergeInputStates 合并。
    return pollKeyboard();
}

PlayerInputState PlayerController::mergeInputStates(
    const PlayerInputState& first, const PlayerInputState& second) {
    PlayerInputState merged;

    merged.moveLeftHeld = first.moveLeftHeld || second.moveLeftHeld;
    merged.moveRightHeld = first.moveRightHeld || second.moveRightHeld;
    merged.jumpPressed = first.jumpPressed || second.jumpPressed;
    merged.meleeAttackPressed =
        first.meleeAttackPressed || second.meleeAttackPressed;
    merged.rangedAttackPressed =
        first.rangedAttackPressed || second.rangedAttackPressed;
    merged.defendHeld = first.defendHeld || second.defendHeld;
    merged.dodgePressed = first.dodgePressed || second.dodgePressed;
    merged.selectShadowSkill1Pressed =
        first.selectShadowSkill1Pressed || second.selectShadowSkill1Pressed;
    merged.useShadowSkill2Pressed =
        first.useShadowSkill2Pressed || second.useShadowSkill2Pressed;

    return merged;
}

HorizontalInputDirection PlayerController::resolveHorizontalDirection(
    const PlayerInputState& input) {
    if (input.moveLeftHeld == input.moveRightHeld) {
        // 两者均未按下或同时按下时都不产生移动，也不会改变玩家朝向。
        return HorizontalInputDirection::None;
    }
    return input.moveLeftHeld ? HorizontalInputDirection::Left : HorizontalInputDirection::Right;
}

PlayerInputState PlayerController::pollKeyboard() const {
    PlayerInputState input;

    input.moveLeftHeld = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    input.moveRightHeld = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    input.jumpPressed = IsKeyPressed(KEY_SPACE);
    input.meleeAttackPressed = IsKeyPressed(KEY_J);
    input.rangedAttackPressed = IsKeyPressed(KEY_K);
    input.defendHeld = IsKeyDown(KEY_U);
    input.dodgePressed = IsKeyPressed(KEY_L);
    input.selectShadowSkill1Pressed = IsKeyPressed(KEY_ONE);
    input.useShadowSkill2Pressed = IsKeyPressed(KEY_TWO);

    return input;
}
