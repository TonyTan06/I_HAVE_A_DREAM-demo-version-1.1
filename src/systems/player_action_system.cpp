#include "systems/player_action_system.h"

#include "entities/player.h"

PlayerActionFrameResult PlayerActionSystem::applyActions(
    Player& player,
    const PlayerInputState& input,
    float deltaTime) const {
    const HorizontalDirection horizontalDirection =
        resolveHorizontalDirection(input);
    PlayerActionFrameResult result;
    result.playerMeleeAttackRequested = applyCharacterActions(
        player, input, horizontalDirection, deltaTime);
    return result;
}

PlayerActionSystem::HorizontalDirection
PlayerActionSystem::resolveHorizontalDirection(
    const PlayerInputState& input) {
    if (input.moveLeftHeld == input.moveRightHeld) {
        // 两者均未按下或同时按下时都不移动，也不改变角色朝向。
        return HorizontalDirection::None;
    }
    return input.moveLeftHeld
        ? HorizontalDirection::Left
        : HorizontalDirection::Right;
}

bool PlayerActionSystem::applyCharacterActions(
    Player& character,
    const PlayerInputState& input,
    HorizontalDirection horizontalDirection,
    float deltaTime) {
    if (!character.isDodging()) {
        if (horizontalDirection == HorizontalDirection::Left) {
            character.moveLeft(deltaTime);
        } else if (horizontalDirection == HorizontalDirection::Right) {
            character.moveRight(deltaTime);
        } else {
            character.stopMovingHorizontally();
        }
    }
    if (input.jumpPressed) {
        character.jump();
    }

    character.setDefending(input.defendHeld);
    const bool meleeAttackRequested =
        input.meleeAttackPressed && !character.isDefending();
    if (input.rangedAttackPressed && !character.isDefending()) {
        character.rangedAttack();
    }
    if (input.dodgePressed) {
        character.startDodge(resolveDodgeRight(character, horizontalDirection));
    }

    return meleeAttackRequested;
}

bool PlayerActionSystem::resolveDodgeRight(
    const Player& character,
    HorizontalDirection horizontalDirection) {
    if (horizontalDirection == HorizontalDirection::Right) return true;
    if (horizontalDirection == HorizontalDirection::Left) return false;
    return character.isFacingRight();
}
