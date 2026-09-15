#pragma once

// 一帧内与玩家操作有关的设备无关输入状态。
// 键盘、手柄、AI 或回放系统都应先转换成该结构，再交给 PlayerActionSystem。
struct PlayerInputState {
    bool moveLeftHeld = false; // 本帧是否持续要求玩家向左移动
    bool moveRightHeld = false; // 本帧是否持续要求玩家向右移动
    bool jumpPressed = false; // 本帧是否刚触发一次跳跃
    bool meleeAttackPressed = false; // 本帧是否刚触发一次近战攻击
    bool rangedAttackPressed = false; // 本帧是否刚触发一次远程攻击
    bool defendHeld = false; // 本帧是否持续要求玩家保持防御
    bool dodgePressed = false; // 本帧是否刚触发一次闪避
};
