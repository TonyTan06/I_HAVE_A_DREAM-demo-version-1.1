#include "engine/game_engine.h"

#include "engine/game_config.h"
#include "engine/scene.h"
#include "raylib.h"

#include <cstdlib>
#include <utility>

GameEngine::GameEngine(std::unique_ptr<Scene> initialScene)
    : currentScene_(std::move(initialScene)),
      windowInitialized_(false),
      running_(false) {
}

GameEngine::~GameEngine() {
    // 场景可能持有显存资源，必须先于 raylib 窗口销毁。
    currentScene_.reset();
    shutdown();
}

int GameEngine::run() {
    if (currentScene_ == nullptr) {
        return EXIT_FAILURE;
    }
    if (!initializeWindow()) {
        return EXIT_FAILURE;
    }

    if (!currentScene_->load()) {
        TraceLog(
            LOG_WARNING,
            "The current scene could not load all of its assets");
    }

    running_ = true;
    while (!WindowShouldClose()) {
        currentScene_->update(GetFrameTime());

        BeginDrawing();
        currentScene_->draw();
        EndDrawing();
    }
    running_ = false;

    // 保证纹理等场景资源在 CloseWindow 前释放。
    currentScene_.reset();
    shutdown();
    return EXIT_SUCCESS;
}

bool GameEngine::hasScene() const {
    return currentScene_ != nullptr;
}

bool GameEngine::isRunning() const {
    return running_;
}

bool GameEngine::initializeWindow() {
    InitWindow(
        GameConfig::WINDOW_WIDTH,
        GameConfig::WINDOW_HEIGHT,
        GameConfig::WINDOW_TITLE);
    windowInitialized_ = IsWindowReady();
    if (!windowInitialized_) {
        return false;
    }

    SetTargetFPS(GameConfig::TARGET_FPS);
    return true;
}

void GameEngine::shutdown() {
    running_ = false;
    if (!windowInitialized_) return;

    CloseWindow();
    windowInitialized_ = false;
}
