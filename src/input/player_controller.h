#pragma once

#include "input/input_bindings.h"
#include "input/player_input_state.h"

class PlayerController {
public:
    explicit PlayerController(const InputBindings& bindings);

    // 读取键盘和第一个手柄，并返回合并后的统一输入状态。
    PlayerInputState pollInput() const;

    // 合并两个设备的输入状态；任一设备触发的操作都会被保留。
    static PlayerInputState mergeInputStates(
        const PlayerInputState& first, const PlayerInputState& second);

private:
    const InputBindings& bindings_;

    // 把 raylib 键盘按键转换为设备无关的玩家输入状态。
    PlayerInputState pollKeyboard() const;
    PlayerInputState pollGamepad() const;
    static bool isBindingDown(const KeyBinding& binding);
    static bool isBindingPressed(const KeyBinding& binding);
};
