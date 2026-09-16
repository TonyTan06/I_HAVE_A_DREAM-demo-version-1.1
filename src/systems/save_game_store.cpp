#include "systems/save_game_store.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace {

constexpr const char* GAME_DATA_DIRECTORY = "I_HAVE_A_DREAM";
constexpr const char* SAVE_FILE_NAME = "savegame.sav";

#ifdef _WIN32
std::filesystem::path environmentPath(const wchar_t* variableName) {
    const DWORD requiredSize =
        GetEnvironmentVariableW(variableName, nullptr, 0);
    if (requiredSize == 0) return {};

    std::wstring value(requiredSize, L'\0');
    const DWORD writtenSize = GetEnvironmentVariableW(
        variableName, value.data(), requiredSize);
    if (writtenSize == 0 || writtenSize >= requiredSize) return {};

    value.resize(writtenSize);
    return std::filesystem::path(value);
}
#else
std::filesystem::path environmentPath(const char* variableName) {
    const char* value = std::getenv(variableName);
    if (value == nullptr || value[0] == '\0') return {};

    return std::filesystem::path(value);
}
#endif

bool isValidSnapshot(const SaveGameSnapshot& snapshot) {
    if (snapshot.schemaVersion <= 0 || snapshot.sceneId.empty()) return false;
    if (snapshot.sceneId.find_first_of("\r\n\0", 0, 3) != std::string::npos) {
        return false;
    }
    if (!std::isfinite(snapshot.player.x) ||
        !std::isfinite(snapshot.player.y) ||
        !std::isfinite(snapshot.player.health)) {
        return false;
    }
    return snapshot.player.health >= 0.0F &&
        snapshot.player.level >= 1 &&
        snapshot.player.experience >= 0 &&
        snapshot.player.gold >= 0;
}

std::string serializeSnapshot(const SaveGameSnapshot& snapshot) {
    std::ostringstream output;
    output.imbue(std::locale::classic());
    output << std::setprecision(std::numeric_limits<float>::max_digits10)
           << "format=I_HAVE_A_DREAM_SAVE\n"
           << "schema_version=" << snapshot.schemaVersion << '\n'
           << "scene_id=" << std::quoted(snapshot.sceneId) << '\n'
           << "player_x=" << snapshot.player.x << '\n'
           << "player_y=" << snapshot.player.y << '\n'
           << "player_health=" << snapshot.player.health << '\n'
           << "player_level=" << snapshot.player.level << '\n'
           << "player_experience=" << snapshot.player.experience << '\n'
           << "player_gold=" << snapshot.player.gold << '\n'
           << "player_facing_right="
           << (snapshot.player.facingRight ? "true" : "false") << '\n';
    return output.str();
}

std::filesystem::path makeTemporaryPath(
    const std::filesystem::path& savePath) {
    static std::atomic<unsigned long long> sequence{0};
    const auto timestamp =
        std::chrono::steady_clock::now().time_since_epoch().count();

    std::filesystem::path temporaryPath = savePath;
    temporaryPath += ".tmp-";
    temporaryPath += std::to_string(timestamp);
    temporaryPath += "-";
    temporaryPath += std::to_string(sequence.fetch_add(1));
    return temporaryPath;
}

bool replaceSaveFile(
    const std::filesystem::path& temporaryPath,
    const std::filesystem::path& savePath) {
#ifdef _WIN32
    // std::filesystem::rename 不会在 Windows 上替换已有文件。
    return MoveFileExW(
               temporaryPath.c_str(),
               savePath.c_str(),
               MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    // 临时文件与正式存档在同一目录；POSIX rename 会原子替换已有文件。
    std::error_code renameError;
    std::filesystem::rename(temporaryPath, savePath, renameError);
    return !renameError;
#endif
}

void removeTemporaryFile(const std::filesystem::path& temporaryPath) {
    std::error_code removeError;
    std::filesystem::remove(temporaryPath, removeError);
}

} // namespace

std::filesystem::path SaveGameStore::getDefaultSavePath() {
    try {
        std::filesystem::path dataDirectory;

#ifdef _WIN32
        dataDirectory = environmentPath(L"APPDATA");
        if (dataDirectory.empty()) {
            const std::filesystem::path userProfile =
                environmentPath(L"USERPROFILE");
            if (!userProfile.empty()) {
                dataDirectory = userProfile / "AppData" / "Roaming";
            }
        }
#elif defined(__APPLE__)
        const std::filesystem::path homeDirectory = environmentPath("HOME");
        if (!homeDirectory.empty()) {
            dataDirectory =
                homeDirectory / "Library" / "Application Support";
        }
#else
        const std::filesystem::path xdgDataDirectory =
            environmentPath("XDG_DATA_HOME");
        if (xdgDataDirectory.is_absolute()) {
            dataDirectory = xdgDataDirectory;
        } else {
            const std::filesystem::path homeDirectory =
                environmentPath("HOME");
            if (!homeDirectory.empty()) {
                dataDirectory = homeDirectory / ".local" / "share";
            }
        }
#endif

        if (dataDirectory.empty()) return {};
        return dataDirectory / GAME_DATA_DIRECTORY / SAVE_FILE_NAME;
    } catch (...) {
        return {};
    }
}

bool SaveGameStore::save(const SaveGameSnapshot& snapshot) {
    return save(snapshot, getDefaultSavePath());
}

bool SaveGameStore::save(
    const SaveGameSnapshot& snapshot,
    const std::filesystem::path& savePath) {
    std::filesystem::path temporaryPath;
    try {
        if (!isValidSnapshot(snapshot) || savePath.empty() ||
            savePath.filename().empty()) {
            return false;
        }

        const std::filesystem::path parentDirectory = savePath.parent_path();
        if (!parentDirectory.empty()) {
            std::error_code directoryError;
            std::filesystem::create_directories(
                parentDirectory, directoryError);
            if (directoryError ||
                !std::filesystem::is_directory(
                    parentDirectory, directoryError) ||
                directoryError) {
                return false;
            }
        }

        temporaryPath = makeTemporaryPath(savePath);
        const std::string contents = serializeSnapshot(snapshot);
        std::ofstream output(
            temporaryPath,
            std::ios::binary | std::ios::out | std::ios::trunc);
        if (!output.is_open()) return false;

        output.write(
            contents.data(), static_cast<std::streamsize>(contents.size()));
        output.flush();
        if (!output.good()) {
            output.close();
            removeTemporaryFile(temporaryPath);
            return false;
        }
        output.close();
        if (output.fail()) {
            removeTemporaryFile(temporaryPath);
            return false;
        }

        if (!replaceSaveFile(temporaryPath, savePath)) {
            removeTemporaryFile(temporaryPath);
            return false;
        }
        return true;
    } catch (...) {
        if (!temporaryPath.empty()) removeTemporaryFile(temporaryPath);
        return false;
    }
}
