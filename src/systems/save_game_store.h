#pragma once

#include <filesystem>
#include <string>

// 玩家在存档中的稳定状态。短时输入、技能冷却和弹道等战斗瞬时状态不写入存档。
struct PlayerSaveSnapshot {
    float x = 0.0F;
    float y = 0.0F;
    float health = 0.0F;
    int level = 1;
    int experience = 0;
    int gold = 0;
    bool facingRight = true;
};

// 一次保存操作使用的完整只读快照。schemaVersion 为以后迁移旧存档预留。
struct SaveGameSnapshot {
    int schemaVersion = 1;
    std::string sceneId;
    PlayerSaveSnapshot player;
};

// 将存档快照写入用户数据目录或调用方指定的位置。
class SaveGameStore final {
public:
    // macOS 使用 Application Support，Windows 使用 AppData，Linux 遵循 XDG。
    static std::filesystem::path getDefaultSavePath();

    // 保存到默认用户数据路径。
    static bool save(const SaveGameSnapshot& snapshot);
    // 保存到指定路径，主要供测试及未来的多存档槽使用。
    static bool save(
        const SaveGameSnapshot& snapshot,
        const std::filesystem::path& savePath);
};
