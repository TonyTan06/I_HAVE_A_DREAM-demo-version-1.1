#include "input/player_controller.h"

#include "raylib.h"

namespace {

constexpr int GAMEPAD_INDEX = 0;
constexpr float GAMEPAD_DEADZONE = 0.2F;

} // namespace

PlayerController::PlayerController(const InputBindings& bindings)
    : bindings_(bindings) {
}

PlayerInputState PlayerController::pollInput() const {
    const PlayerInputState keyboardInput = pollKeyboard();
    const PlayerInputState gamepadInput = pollGamepad();
    return mergeInputStates(keyboardInput, gamepadInput);
}

PlayerInputState PlayerController::mergeInputStates(
    const PlayerInputState& first, const PlayerInputState& second) {
    PlayerInputState merged;

    merged.moveLeftHeld = first.moveLeftHeld || second.moveLeftHeld;
    merged.moveRightHeld = first.moveRightHeld || second.moveRightHeld;
    merged.jumpPressed = first.jumpPressed || second.jumpPressed;
    merged.meleeAttackPressed = first.meleeAttackPressed || second.meleeAttackPressed;
    merged.rangedAttackPressed = first.rangedAttackPressed || second.rangedAttackPressed;
    merged.defendHeld = first.defendHeld || second.defendHeld;
    merged.dodgePressed = first.dodgePressed || second.dodgePressed;

    return merged;
}

PlayerInputState PlayerController::pollKeyboard() const {
    PlayerInputState input;

    input.moveLeftHeld = isBindingDown(bindings_.keyboard.moveLeft);
    input.moveRightHeld = isBindingDown(bindings_.keyboard.moveRight);
    input.jumpPressed = isBindingPressed(bindings_.keyboard.jump);
    input.meleeAttackPressed = isBindingPressed(bindings_.keyboard.meleeAttack);
    input.rangedAttackPressed = isBindingPressed(bindings_.keyboard.rangedAttack);
    input.defendHeld = isBindingDown(bindings_.keyboard.defend);
    input.dodgePressed = isBindingPressed(bindings_.keyboard.dodge);

    return input;
}

PlayerInputState PlayerController::pollGamepad() const {
    if (!IsGamepadAvailable(GAMEPAD_INDEX)) return PlayerInputState{};

    PlayerInputState input;
    const float axisX = GetGamepadAxisMovement(
        GAMEPAD_INDEX, GAMEPAD_AXIS_LEFT_X);

    input.moveLeftHeld = axisX < -GAMEPAD_DEADZONE ||
        IsGamepadButtonDown(GAMEPAD_INDEX, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
    input.moveRightHeld = axisX > GAMEPAD_DEADZONE ||
        IsGamepadButtonDown(GAMEPAD_INDEX, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
    input.jumpPressed = IsGamepadButtonPressed(
        GAMEPAD_INDEX, bindings_.gamepad.jumpButton);
    input.meleeAttackPressed = IsGamepadButtonPressed(
        GAMEPAD_INDEX, bindings_.gamepad.meleeAttackButton);
    input.rangedAttackPressed = IsGamepadButtonPressed(
        GAMEPAD_INDEX, bindings_.gamepad.rangedAttackButton);
    input.defendHeld = IsGamepadButtonDown(
        GAMEPAD_INDEX, bindings_.gamepad.defendButton);
    input.dodgePressed = IsGamepadButtonPressed(
        GAMEPAD_INDEX, bindings_.gamepad.dodgeButton);

    return input;
}

bool PlayerController::isBindingDown(const KeyBinding& binding) {
    return (binding.primary != KEY_NULL && IsKeyDown(binding.primary)) ||
        (binding.secondary != KEY_NULL && IsKeyDown(binding.secondary));
}

bool PlayerController::isBindingPressed(const KeyBinding& binding) {
    return (binding.primary != KEY_NULL && IsKeyPressed(binding.primary)) ||
        (binding.secondary != KEY_NULL && IsKeyPressed(binding.secondary));
}
