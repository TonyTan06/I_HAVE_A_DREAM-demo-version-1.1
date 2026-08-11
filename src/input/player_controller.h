#pragma once

// 一帧内与玩家操作有关的设备无关输入状态。
// 键盘、手柄或后续设备都应先转换成该结构，再交给 Scene 使用。
struct PlayerInputState {
    bool moveLeftHeld = false; // 本帧是否持续要求玩家向左移动
    bool moveRightHeld = false; // 本帧是否持续要求玩家向右移动
    bool jumpPressed = false; // 本帧是否刚触发一次跳跃
    bool meleeAttackPressed = false; // 本帧是否刚触发一次近战攻击
    bool rangedAttackPressed = false; // 本帧是否刚触发一次远程攻击
    bool defendHeld = false; // 本帧是否持续要求玩家保持防御
    bool dodgePressed = false; // 本帧是否刚触发一次闪避
    bool selectShadowSkill1Pressed = false; // 本帧是否按数字键 1 选择影子技能 1
    bool useShadowSkill2Pressed = false; // 本帧是否按数字键 2 尝试使用影子技能 2
};

// 解析左右输入冲突后的水平移动方向。
enum class HorizontalInputDirection {
    None, // 没有按方向键，或左右方向同时按下
    Left, // 仅要求向左移动
    Right // 仅要求向右移动
};

class PlayerController {
public:
    // 读取当前已接入的输入设备并返回统一状态；目前仅接入键盘。
    PlayerInputState pollInput() const;

    // 合并两个设备的输入状态；任一设备触发的操作都会被保留。
    static PlayerInputState mergeInputStates(
        const PlayerInputState& first, const PlayerInputState& second);

    // 同时存在左右输入时返回 None，保证玩家停止且不改变当前朝向。
    static HorizontalInputDirection resolveHorizontalDirection(
        const PlayerInputState& input);

private:
    // 把 raylib 键盘按键转换为设备无关的玩家输入状态。
    PlayerInputState pollKeyboard() const;
};
