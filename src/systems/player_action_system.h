#pragma once

#include "input/player_input_state.h"

class Player;

// GameWorld 用该结果继续处理命中判定，并向 Scene 提供视觉反馈。
struct PlayerActionFrameResult {
    bool playerMeleeAttackRequested = false;
};

// 把设备无关的操作意图解释为角色领域动作。
// Player 不接触输入设备、按键或 PlayerInputState。
class PlayerActionSystem {
public:
    PlayerActionFrameResult applyActions(
        Player& player,
        const PlayerInputState& input,
        float deltaTime) const;

private:
    enum class HorizontalDirection {
        None,
        Left,
        Right
    };

    static HorizontalDirection resolveHorizontalDirection(
        const PlayerInputState& input);
    static bool applyCharacterActions(
        Player& character,
        const PlayerInputState& input,
        HorizontalDirection horizontalDirection,
        float deltaTime);
    static bool resolveDodgeRight(
        const Player& character,
        HorizontalDirection horizontalDirection);
};
