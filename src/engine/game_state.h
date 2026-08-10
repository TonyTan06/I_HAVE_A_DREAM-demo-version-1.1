#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>

// GameState 只描述整个游戏当前所处的高层流程阶段。
// 角色移动、攻击、死亡等实体行为不属于这里，应由各自实体或系统管理。
enum class GameState {
    Booting,
    MainMenu,
    Loading,
    Playing,
    Paused,
    Victory,
    Defeat,
    Exiting
};

// 每次高层状态真正发生变化时发送给监听者，例如未来的音乐或 UI 系统。
struct GameStateChangedEvent {
    GameState previousState;
    GameState currentState;
};

// 保存当前 GameState，并将状态变化同步通知给已注册的监听者。
// 当前项目在主线程使用该对象，不负责线程同步或限制具体的状态转换路径。
class GameStateManager {
public:
    using ListenerId = std::size_t;
    using StateChangedListener =
        std::function<void(const GameStateChangedEvent&)>;

    explicit GameStateManager(GameState initialState);

    GameState getCurrentState() const;
    // 状态不变时返回 false 且不重复发送事件。
    bool transitionTo(GameState nextState);
    ListenerId addStateChangedListener(StateChangedListener listener) const;
    bool removeStateChangedListener(ListenerId listenerId) const;

private:
    struct ListenerRegistration {
        ListenerId id;
        StateChangedListener listener;
    };

    GameState currentState_;
    std::deque<GameState> pendingStates_;
    bool isDispatching_;
    mutable std::vector<ListenerRegistration> listeners_;
    mutable ListenerId nextListenerId_;

    std::size_t findListenerIndex(ListenerId listenerId) const;
};
