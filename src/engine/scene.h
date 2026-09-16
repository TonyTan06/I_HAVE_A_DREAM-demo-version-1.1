#pragma once

enum class SceneSaveResult {
    Success,
    Failed,
    Unsupported
};

// GameEngine 只依赖这个通用接口，不需要知道当前运行的是游戏、菜单或结算场景。
class Scene {
public:
    virtual ~Scene() = default;

    // 需要 GPU/窗口上下文的资源在 GameEngine 创建窗口后加载。
    virtual bool load() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw() const = 0;
    // 支持存档的场景覆写此函数；默认明确返回“不支持”。
    virtual SceneSaveResult save() const {
        return SceneSaveResult::Unsupported;
    }
};
