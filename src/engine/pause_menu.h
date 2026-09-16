#pragma once

#include <string>

enum class PauseMenuPage {
    Main,
    Settings
};

enum class PauseMenuAction {
    None,
    Resume,
    Save,
    SaveAndExit,
    ToggleFullscreen
};

// 与具体设备无关的一帧菜单输入，方便后续接入手柄并进行无窗口测试。
struct PauseMenuInput {
    bool cancelPressed = false;
    bool previousPressed = false;
    bool nextPressed = false;
    bool confirmPressed = false;
    int hoveredOption = -1;
    int activatedOption = -1;
};

// 管理暂停菜单的页面、选项和反馈；绘制时覆盖在仍然可见的游戏场景上。
class PauseMenu {
public:
    PauseMenu();

    void open();
    PauseMenuAction updateFromDevices(float deltaTime);
    PauseMenuAction handleInput(
        const PauseMenuInput& input, float deltaTime = 0.0F);
    void draw(bool isFullscreen) const;
    void showSaveResult(bool success);

    PauseMenuPage getPage() const;
    int getSelectedOptionIndex() const;

private:
    PauseMenuPage page_;
    int selectedOptionIndex_;
    std::string feedbackMessage_;
    float feedbackRemainingTime_;
    bool feedbackIsSuccess_;
    float previousMouseX_;
    float previousMouseY_;
    bool hasPreviousMousePosition_;

    int getOptionCount() const;
    PauseMenuAction activateSelectedOption();
};
