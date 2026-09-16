#include "engine/pause_menu.h"

#include <gtest/gtest.h>

TEST(PauseMenuTest, OpensOnMainPageWithResumeSelected) {
    PauseMenu menu;

    menu.open();

    EXPECT_EQ(menu.getPage(), PauseMenuPage::Main);
    EXPECT_EQ(menu.getSelectedOptionIndex(), 0);
}

TEST(PauseMenuTest, WrapsKeyboardSelectionInBothDirections) {
    PauseMenu menu;
    PauseMenuInput input;
    input.previousPressed = true;

    menu.handleInput(input);
    EXPECT_EQ(menu.getSelectedOptionIndex(), 3);

    input.previousPressed = false;
    input.nextPressed = true;
    menu.handleInput(input);
    EXPECT_EQ(menu.getSelectedOptionIndex(), 0);
}

TEST(PauseMenuTest, ReturnsActionsForResumeSaveAndSaveAndExit) {
    PauseMenu menu;
    PauseMenuInput input;
    input.confirmPressed = true;
    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::Resume);

    input.confirmPressed = false;
    input.activatedOption = 2;
    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::Save);

    input.activatedOption = 3;
    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::SaveAndExit);
}

TEST(PauseMenuTest, SettingsRemainInsidePausedMenuAndCanReturn) {
    PauseMenu menu;
    PauseMenuInput input;
    input.activatedOption = 1;

    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::None);
    EXPECT_EQ(menu.getPage(), PauseMenuPage::Settings);
    EXPECT_EQ(menu.getSelectedOptionIndex(), 0);

    input = PauseMenuInput{};
    input.confirmPressed = true;
    EXPECT_EQ(
        menu.handleInput(input), PauseMenuAction::ToggleFullscreen);

    input = PauseMenuInput{};
    input.cancelPressed = true;
    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::None);
    EXPECT_EQ(menu.getPage(), PauseMenuPage::Main);
    EXPECT_EQ(menu.getSelectedOptionIndex(), 1);
}

TEST(PauseMenuTest, SettingsBackOptionReturnsToMainPage) {
    PauseMenu menu;
    PauseMenuInput input;
    input.activatedOption = 1;
    menu.handleInput(input);

    input.activatedOption = -1;
    input.nextPressed = true;
    menu.handleInput(input);
    input.nextPressed = false;
    input.confirmPressed = true;
    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::None);
    EXPECT_EQ(menu.getPage(), PauseMenuPage::Main);
    EXPECT_EQ(menu.getSelectedOptionIndex(), 1);
}

TEST(PauseMenuTest, MouseHoverSelectsAndMouseClickActivatesAnOption) {
    PauseMenu menu;
    PauseMenuInput input;
    input.hoveredOption = 2;

    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::None);
    EXPECT_EQ(menu.getSelectedOptionIndex(), 2);

    input = PauseMenuInput{};
    input.activatedOption = 2;
    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::Save);
}

TEST(PauseMenuTest, EscapeFromMainPageResumesTheGame) {
    PauseMenu menu;
    PauseMenuInput input;
    input.cancelPressed = true;

    EXPECT_EQ(menu.handleInput(input), PauseMenuAction::Resume);
}
