#include "systems/save_game_store.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace {

class UniqueTemporaryDirectory final {
public:
    UniqueTemporaryDirectory() {
        static std::atomic<unsigned long long> sequence{0};
        const auto timestamp =
            std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path() /
            ("i_have_a_dream_save_test_" + std::to_string(timestamp) + "_" +
             std::to_string(sequence.fetch_add(1)));
        std::filesystem::create_directories(path_);
    }

    ~UniqueTemporaryDirectory() {
        std::error_code removeError;
        std::filesystem::remove_all(path_, removeError);
    }

    const std::filesystem::path& path() const {
        return path_;
    }

private:
    std::filesystem::path path_;
};

SaveGameSnapshot makeSnapshot() {
    SaveGameSnapshot snapshot;
    snapshot.sceneId = "game_scene";
    snapshot.player.x = 12.5F;
    snapshot.player.y = 3.25F;
    snapshot.player.health = 7.75F;
    snapshot.player.level = 4;
    snapshot.player.experience = 23;
    snapshot.player.gold = 81;
    snapshot.player.facingRight = false;
    return snapshot;
}

std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    return std::string(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());
}

} // namespace

TEST(SaveGameStoreTest, SnapshotUsesTheFirstSchemaVersionByDefault) {
    const SaveGameSnapshot snapshot;

    EXPECT_EQ(snapshot.schemaVersion, 1);
}

TEST(SaveGameStoreTest, CreatesDirectoriesAndSafelyReplacesExistingSave) {
    const UniqueTemporaryDirectory temporaryDirectory;
    const std::filesystem::path savePath =
        temporaryDirectory.path() / "nested" / "slot-1.sav";
    SaveGameSnapshot snapshot = makeSnapshot();

    ASSERT_TRUE(SaveGameStore::save(snapshot, savePath));
    EXPECT_EQ(
        readFile(savePath),
        "format=I_HAVE_A_DREAM_SAVE\n"
        "schema_version=1\n"
        "scene_id=\"game_scene\"\n"
        "player_x=12.5\n"
        "player_y=3.25\n"
        "player_health=7.75\n"
        "player_level=4\n"
        "player_experience=23\n"
        "player_gold=81\n"
        "player_facing_right=false\n");

    snapshot.player.health = 2.0F;
    snapshot.player.gold = 99;
    snapshot.player.facingRight = true;
    ASSERT_TRUE(SaveGameStore::save(snapshot, savePath));

    const std::string replacedContents = readFile(savePath);
    EXPECT_NE(replacedContents.find("player_health=2\n"), std::string::npos);
    EXPECT_NE(replacedContents.find("player_gold=99\n"), std::string::npos);
    EXPECT_NE(
        replacedContents.find("player_facing_right=true\n"),
        std::string::npos);

    std::size_t fileCount = 0;
    for (const auto& entry :
         std::filesystem::directory_iterator(savePath.parent_path())) {
        (void)entry;
        ++fileCount;
    }
    EXPECT_EQ(fileCount, 1U);
}

TEST(SaveGameStoreTest, ReturnsFalseWhenParentPathIsARegularFile) {
    const UniqueTemporaryDirectory temporaryDirectory;
    const std::filesystem::path invalidParent =
        temporaryDirectory.path() / "not-a-directory";
    {
        std::ofstream parentFile(invalidParent, std::ios::binary);
        ASSERT_TRUE(parentFile.is_open());
        parentFile << "keep this file";
    }

    EXPECT_FALSE(SaveGameStore::save(
        makeSnapshot(), invalidParent / "slot-1.sav"));
    EXPECT_EQ(readFile(invalidParent), "keep this file");
}
