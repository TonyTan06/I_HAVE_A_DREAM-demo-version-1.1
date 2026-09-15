#pragma once

#include "raylib.h"

// 一项键盘操作可绑定两个按键；KEY_NULL 表示该位置未绑定。
struct KeyBinding {
    KeyboardKey primary = KEY_NULL;
    KeyboardKey secondary = KEY_NULL;
};

struct KeyboardBindings {
    KeyBinding moveLeft{KEY_A, KEY_LEFT};
    KeyBinding moveRight{KEY_D, KEY_RIGHT};
    KeyBinding jump{KEY_SPACE, KEY_NULL};
    KeyBinding meleeAttack{KEY_J, KEY_NULL};
    KeyBinding rangedAttack{KEY_K, KEY_NULL};
    KeyBinding defend{KEY_U, KEY_NULL};
    KeyBinding dodge{KEY_L, KEY_NULL};
};

// 移动固定使用左摇杆 X 轴和 D-Pad 左右；这里只保存按钮型动作。
struct GamepadBindings {
    GamepadButton jumpButton = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
    GamepadButton meleeAttackButton = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
    GamepadButton rangedAttackButton = GAMEPAD_BUTTON_RIGHT_FACE_UP;
    GamepadButton defendButton = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
    GamepadButton dodgeButton = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
};

struct InputBindings {
    KeyboardBindings keyboard;
    GamepadBindings gamepad;
};
