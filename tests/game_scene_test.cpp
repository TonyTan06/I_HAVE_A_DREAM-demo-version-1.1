#include "scenes/game_scene.h"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>

namespace {

std::filesystem::path makeSavePath(const std::string& scenario) {
    const auto uniqueSuffix =
        std::chrono::steady_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path() /
        ("i_have_a_dream_" + scenario + "_" +
         std::to_string(uniqueSuffix) + ".sav");
}

std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input.is_open()) return {};
    return std::string{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()};
}

float readFloatField(const std::string& contents, const std::string& field) {
    const std::string prefix = field + "=";
    const std::size_t valueStart = contents.find(prefix);
    if (valueStart == std::string::npos) {
        return std::numeric_limits<float>::quiet_NaN();
    }
    const std::size_t lineEnd = contents.find('\n', valueStart);
    return std::stof(contents.substr(
        valueStart + prefix.size(), lineEnd - valueStart - prefix.size()));
}

void removeSaveFile(const std::filesystem::path& path) {
    std::error_code removeError;
    std::filesystem::remove(path, removeError);
}

} // namespace

TEST(GameSceneTest, CanBeCreatedWithoutOpeningAWindow) {
    GameScene scene;

    SUCCEED();
}

TEST(GameSceneTest, SavesPlayerProgressToAnInjectedPath) {
    const std::filesystem::path savePath = makeSavePath("scene_save_test");
    GameScene scene(savePath);

    ASSERT_EQ(scene.save(), SceneSaveResult::Success);

    const std::string contents = readFile(savePath);
    ASSERT_FALSE(contents.empty());
    EXPECT_NE(contents.find("scene_id=\"game_scene\"\n"), std::string::npos);
    EXPECT_NE(contents.find("player_x=140\n"), std::string::npos);
    EXPECT_NE(contents.find("player_level=1\n"), std::string::npos);
    EXPECT_NE(contents.find("player_experience=0\n"), std::string::npos);
    EXPECT_NE(contents.find("player_gold=0\n"), std::string::npos);

    removeSaveFile(savePath);
}

TEST(GameSceneTest, InjectedJumpEntersPhysicsDuringTheSameFrame) {
    const std::filesystem::path savePath = makeSavePath("jump_update_test");
    GameScene scene(savePath);
    PlayerInputState input;
    input.jumpPressed = true;

    scene.updateWithInput(input, 0.1F);
    ASSERT_EQ(scene.save(), SceneSaveResult::Success);

    const std::string contents = readFile(savePath);
    ASSERT_FALSE(contents.empty());
    EXPECT_NEAR(readFloatField(contents, "player_y"), 26.2F, 0.001F);

    removeSaveFile(savePath);
}

TEST(GameSceneTest, PlayerMovementIsClampedDuringTheSameFrame) {
    const std::filesystem::path savePath = makeSavePath("clamp_update_test");
    GameScene scene(savePath);
    PlayerInputState input;
    input.moveLeftHeld = true;

    scene.updateWithInput(input, 1.0F);
    ASSERT_EQ(scene.save(), SceneSaveResult::Success);

    const std::string contents = readFile(savePath);
    ASSERT_FALSE(contents.empty());
    EXPECT_FLOAT_EQ(readFloatField(contents, "player_x"), 0.0F);

    removeSaveFile(savePath);
}
