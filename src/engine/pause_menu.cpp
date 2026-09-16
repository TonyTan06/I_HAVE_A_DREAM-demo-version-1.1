#include "engine/pause_menu.h"

#include "raylib.h"

#include <algorithm>
#include <array>
#include <string>

namespace {

constexpr int MAIN_OPTION_COUNT = 4;
constexpr int SETTINGS_OPTION_COUNT = 2;
constexpr float PANEL_WIDTH = 480.0F;
constexpr float MAIN_PANEL_HEIGHT = 360.0F;
constexpr float SETTINGS_PANEL_HEIGHT = 280.0F;
constexpr float BUTTON_WIDTH = 360.0F;
constexpr float BUTTON_HEIGHT = 48.0F;
constexpr float BUTTON_GAP = 12.0F;
constexpr float FEEDBACK_LIFETIME = 2.5F;

const std::array<const char*, MAIN_OPTION_COUNT> MAIN_OPTION_LABELS{
    "RESUME",
    "SETTINGS",
    "SAVE",
    "SAVE & QUIT"};

Rectangle getPanelRectangle(PauseMenuPage page) {
    const float panelHeight = page == PauseMenuPage::Main
        ? MAIN_PANEL_HEIGHT
        : SETTINGS_PANEL_HEIGHT;
    return Rectangle{
        (static_cast<float>(GetScreenWidth()) - PANEL_WIDTH) / 2.0F,
        (static_cast<float>(GetScreenHeight()) - panelHeight) / 2.0F,
        PANEL_WIDTH,
        panelHeight};
}

Rectangle getOptionRectangle(PauseMenuPage page, int optionIndex) {
    const Rectangle panel = getPanelRectangle(page);
    const int optionCount = page == PauseMenuPage::Main
        ? MAIN_OPTION_COUNT
        : SETTINGS_OPTION_COUNT;
    const float optionsHeight =
        optionCount * BUTTON_HEIGHT + (optionCount - 1) * BUTTON_GAP;
    const float firstOptionY =
        panel.y + (panel.height - optionsHeight) / 2.0F + 24.0F;
    return Rectangle{
        panel.x + (panel.width - BUTTON_WIDTH) / 2.0F,
        firstOptionY + optionIndex * (BUTTON_HEIGHT + BUTTON_GAP),
        BUTTON_WIDTH,
        BUTTON_HEIGHT};
}

int hitTestOption(PauseMenuPage page, float mouseX, float mouseY) {
    const int optionCount = page == PauseMenuPage::Main
        ? MAIN_OPTION_COUNT
        : SETTINGS_OPTION_COUNT;
    for (int optionIndex = 0; optionIndex < optionCount; ++optionIndex) {
        if (CheckCollisionPointRec(
                Vector2{mouseX, mouseY},
                getOptionRectangle(page, optionIndex))) {
            return optionIndex;
        }
    }
    return -1;
}

void drawCenteredText(
    const std::string& text, int y, int fontSize, Color color) {
    const int textWidth = MeasureText(text.c_str(), fontSize);
    DrawText(
        text.c_str(),
        (GetScreenWidth() - textWidth) / 2,
        y,
        fontSize,
        color);
}

} // namespace

PauseMenu::PauseMenu()
    : page_(PauseMenuPage::Main),
      selectedOptionIndex_(0),
      feedbackRemainingTime_(0.0F),
      feedbackIsSuccess_(false),
      previousMouseX_(0.0F),
      previousMouseY_(0.0F),
      hasPreviousMousePosition_(false) {
}

void PauseMenu::open() {
    page_ = PauseMenuPage::Main;
    selectedOptionIndex_ = 0;
    feedbackMessage_.clear();
    feedbackRemainingTime_ = 0.0F;
    hasPreviousMousePosition_ = false;
}

PauseMenuAction PauseMenu::updateFromDevices(float deltaTime) {
    const Vector2 mousePosition = GetMousePosition();
    const bool mouseMoved = !hasPreviousMousePosition_ ||
        mousePosition.x != previousMouseX_ ||
        mousePosition.y != previousMouseY_;

    PauseMenuInput input;
    input.cancelPressed = IsKeyPressed(KEY_ESCAPE);
    input.previousPressed =
        IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    input.nextPressed =
        IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
    input.confirmPressed =
        IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    if (mouseMoved) {
        input.hoveredOption =
            hitTestOption(page_, mousePosition.x, mousePosition.y);
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        input.activatedOption =
            hitTestOption(page_, mousePosition.x, mousePosition.y);
    }

    previousMouseX_ = mousePosition.x;
    previousMouseY_ = mousePosition.y;
    hasPreviousMousePosition_ = true;
    return handleInput(input, deltaTime);
}

PauseMenuAction PauseMenu::handleInput(
    const PauseMenuInput& input, float deltaTime) {
    feedbackRemainingTime_ =
        std::max(0.0F, feedbackRemainingTime_ - deltaTime);

    if (input.cancelPressed) {
        if (page_ == PauseMenuPage::Settings) {
            page_ = PauseMenuPage::Main;
            selectedOptionIndex_ = 1;
            return PauseMenuAction::None;
        }
        return PauseMenuAction::Resume;
    }

    const int optionCount = getOptionCount();
    if (input.hoveredOption >= 0 && input.hoveredOption < optionCount) {
        selectedOptionIndex_ = input.hoveredOption;
    }
    if (input.previousPressed) {
        selectedOptionIndex_ =
            (selectedOptionIndex_ + optionCount - 1) % optionCount;
    }
    if (input.nextPressed) {
        selectedOptionIndex_ = (selectedOptionIndex_ + 1) % optionCount;
    }

    if (input.activatedOption >= 0 &&
        input.activatedOption < optionCount) {
        selectedOptionIndex_ = input.activatedOption;
        return activateSelectedOption();
    }
    if (input.confirmPressed) {
        return activateSelectedOption();
    }
    return PauseMenuAction::None;
}

void PauseMenu::draw(bool isFullscreen) const {
    DrawRectangle(
        0, 0, GetScreenWidth(), GetScreenHeight(),
        Color{0, 0, 0, 165});

    const Rectangle panel = getPanelRectangle(page_);
    DrawRectangleRounded(panel, 0.08F, 8, Color{28, 34, 46, 245});
    DrawRectangleRoundedLinesEx(
        panel, 0.08F, 8, 2.0F, Color{116, 198, 255, 255});

    drawCenteredText(
        page_ == PauseMenuPage::Main ? "PAUSED" : "SETTINGS",
        static_cast<int>(panel.y + 24.0F),
        32,
        RAYWHITE);

    for (int optionIndex = 0; optionIndex < getOptionCount(); ++optionIndex) {
        const bool selected = optionIndex == selectedOptionIndex_;
        const Rectangle optionRectangle =
            getOptionRectangle(page_, optionIndex);
        DrawRectangleRounded(
            optionRectangle,
            0.14F,
            6,
            selected
                ? Color{43, 111, 154, 255}
                : Color{48, 57, 72, 255});
        DrawRectangleRoundedLinesEx(
            optionRectangle,
            0.14F,
            6,
            selected ? 2.0F : 1.0F,
            selected ? SKYBLUE : Color{86, 96, 112, 255});

        std::string label;
        if (page_ == PauseMenuPage::Main) {
            label = MAIN_OPTION_LABELS[static_cast<std::size_t>(optionIndex)];
        } else if (optionIndex == 0) {
            label = std::string("FULLSCREEN: ") +
                (isFullscreen ? "ON" : "OFF");
        } else {
            label = "BACK";
        }
        const int labelWidth = MeasureText(label.c_str(), 22);
        DrawText(
            label.c_str(),
            static_cast<int>(
                optionRectangle.x +
                (optionRectangle.width - labelWidth) / 2.0F),
            static_cast<int>(optionRectangle.y + 13.0F),
            22,
            selected ? WHITE : LIGHTGRAY);
    }

    if (feedbackRemainingTime_ > 0.0F && !feedbackMessage_.empty()) {
        drawCenteredText(
            feedbackMessage_,
            static_cast<int>(panel.y + panel.height - 30.0F),
            18,
            feedbackIsSuccess_ ? GREEN : ORANGE);
    }

    drawCenteredText(
        "W/S or arrows: navigate    Enter: select    Esc: back",
        GetScreenHeight() - 30,
        16,
        LIGHTGRAY);
}

void PauseMenu::showSaveResult(bool success) {
    feedbackIsSuccess_ = success;
    feedbackMessage_ = success
        ? "GAME SAVED"
        : "SAVE FAILED - GAME NOT CLOSED";
    feedbackRemainingTime_ = FEEDBACK_LIFETIME;
}

PauseMenuPage PauseMenu::getPage() const {
    return page_;
}

int PauseMenu::getSelectedOptionIndex() const {
    return selectedOptionIndex_;
}

int PauseMenu::getOptionCount() const {
    return page_ == PauseMenuPage::Main
        ? MAIN_OPTION_COUNT
        : SETTINGS_OPTION_COUNT;
}

PauseMenuAction PauseMenu::activateSelectedOption() {
    if (page_ == PauseMenuPage::Settings) {
        if (selectedOptionIndex_ == 0) {
            return PauseMenuAction::ToggleFullscreen;
        }

        page_ = PauseMenuPage::Main;
        selectedOptionIndex_ = 1;
        return PauseMenuAction::None;
    }

    switch (selectedOptionIndex_) {
        case 0:
            return PauseMenuAction::Resume;
        case 1:
            page_ = PauseMenuPage::Settings;
            selectedOptionIndex_ = 0;
            return PauseMenuAction::None;
        case 2:
            return PauseMenuAction::Save;
        case 3:
            return PauseMenuAction::SaveAndExit;
        default:
            return PauseMenuAction::None;
    }
}
