#pragma once

#include "engine/game_state.h"

// 测试中复用的具名状态监听 helper，避免各测试重复定义匿名回调。
struct StateChangeCounter {
    int& notificationCount;

    void operator()(const GameStateChangedEvent&) const {
        ++notificationCount;
    }
};

struct StateChangeRecorder {
    GameStateChangedEvent& receivedEvent;
    int& notificationCount;

    void operator()(const GameStateChangedEvent& event) const {
        receivedEvent = event;
        ++notificationCount;
    }
};
