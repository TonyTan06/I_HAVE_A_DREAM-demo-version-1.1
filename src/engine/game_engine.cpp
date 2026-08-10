#include "engine/game_engine.h"

#include "engine/game_config.h"
#include "engine/scene.h"
#include "raylib.h"

#include <cstdlib>
#include <utility>

GameEngine::GameEngine(std::unique_ptr<Scene> initialScene)
    : currentScene_(std::move(initialScene)),
      gameStateManager_(GameState::Booting),
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
        gameStateManager_.transitionTo(GameState::Exiting);
        return EXIT_FAILURE;
    }
    if (!initializeWindow()) {
        gameStateManager_.transitionTo(GameState::Exiting);
        return EXIT_FAILURE;
    }

    gameStateManager_.transitionTo(GameState::Loading);
    if (!currentScene_->load()) {
        TraceLog(
            LOG_WARNING,
            "The current scene could not load all of its assets");
    }

    running_ = true;
    gameStateManager_.transitionTo(GameState::Playing);
    while (running_ && !WindowShouldClose()) {
        const float deltaTime = GetFrameTime();
        if (gameStateManager_.getCurrentState() == GameState::Playing) {
            if (IsKeyPressed(KEY_ESCAPE)) {
                pauseMenu_.open();
                gameStateManager_.transitionTo(GameState::Paused);
            } else {
                currentScene_->update(deltaTime);
            }
        } else if (
            gameStateManager_.getCurrentState() == GameState::Paused) {
            handlePauseMenuAction(pauseMenu_.updateFromDevices(deltaTime));
        }

        if (!running_) break;

        BeginDrawing();
        currentScene_->draw();
        if (gameStateManager_.getCurrentState() == GameState::Paused) {
            pauseMenu_.draw(IsWindowFullscreen());
        }
        EndDrawing();
    }
    running_ = false;
    gameStateManager_.transitionTo(GameState::Exiting);

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

GameState GameEngine::getGameState() const {
    return gameStateManager_.getCurrentState();
}

const GameStateManager& GameEngine::getGameStateManager() const {
    return gameStateManager_;
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
    // raylib 默认将 Esc 作为退出键；关闭该默认行为后由暂停菜单接管。
    SetExitKey(KEY_NULL);
    return true;
}

void GameEngine::handlePauseMenuAction(PauseMenuAction action) {
    switch (action) {
        case PauseMenuAction::None:
            return;
        case PauseMenuAction::Resume:
            gameStateManager_.transitionTo(GameState::Playing);
            return;
        case PauseMenuAction::Save: {
            const SceneSaveResult result = currentScene_->save();
            pauseMenu_.showSaveResult(result == SceneSaveResult::Success);
            return;
        }
        case PauseMenuAction::SaveAndExit: {
            const SceneSaveResult result = currentScene_->save();
            if (result == SceneSaveResult::Success) {
                running_ = false;
            } else {
                pauseMenu_.showSaveResult(false);
            }
            return;
        }
        case PauseMenuAction::ToggleFullscreen:
            ToggleFullscreen();
            return;
    }
}

void GameEngine::shutdown() {
    running_ = false;
    if (!windowInitialized_) return;

    CloseWindow();
    windowInitialized_ = false;
}
