#pragma once

#include <memory>

class Scene;

// GameEngine 管理窗口、主循环和当前场景的生命周期。
// 具体游戏规则由注入的 Scene 实现。
class GameEngine {
public:
    explicit GameEngine(std::unique_ptr<Scene> initialScene);
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    int run();
    bool hasScene() const;
    bool isRunning() const;

private:
    std::unique_ptr<Scene> currentScene_;
    bool windowInitialized_;
    bool running_;

    bool initializeWindow();
    void shutdown();
};
