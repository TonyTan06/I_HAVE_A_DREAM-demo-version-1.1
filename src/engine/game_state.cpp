#include "engine/game_state.h"

#include <stdexcept>
#include <utility>

GameStateManager::GameStateManager(GameState initialState)
    : currentState_(initialState),
      isDispatching_(false),
      nextListenerId_(1) {
}

GameState GameStateManager::getCurrentState() const {
    return currentState_;
}

bool GameStateManager::transitionTo(GameState nextState) {
    const GameState lastScheduledState = pendingStates_.empty() ? currentState_ : pendingStates_.back();
    if (nextState == lastScheduledState) return false;

    pendingStates_.push_back(nextState);
    if (isDispatching_) return true;

    isDispatching_ = true;
    try {
        while (!pendingStates_.empty()) {
            const GameState scheduledState = pendingStates_.front();
            pendingStates_.pop_front();
            if (scheduledState == currentState_) continue;

            const GameStateChangedEvent event{currentState_, scheduledState};
            currentState_ = scheduledState;

            // 只快照 ID；每次调用前重新确认监听仍存在，取消监听立即生效。
            std::vector<ListenerId> listenerIds;
            listenerIds.reserve(listeners_.size());
            for (const ListenerRegistration& registration : listeners_) {
                listenerIds.push_back(registration.id);
            }
            for (const ListenerId listenerId : listenerIds) {
                const std::size_t listenerIndex =
                    findListenerIndex(listenerId);
                if (listenerIndex == listeners_.size()) continue;

                // 回调可能删除自身，因此在调用前复制函数对象。
                const StateChangedListener listener =
                    listeners_[listenerIndex].listener;
                listener(event);
            }
        }
    } catch (...) {
        pendingStates_.clear();
        isDispatching_ = false;
        throw;
    }

    isDispatching_ = false;
    return true;
}

GameStateManager::ListenerId GameStateManager::addStateChangedListener(
    StateChangedListener listener) const {
    if (!listener) {
        throw std::invalid_argument("State changed listener cannot be empty");
    }

    const ListenerId listenerId = nextListenerId_++;
    listeners_.push_back(ListenerRegistration{listenerId, std::move(listener)});
    return listenerId;
}

bool GameStateManager::removeStateChangedListener(ListenerId listenerId) const {
    const std::size_t listenerIndex = findListenerIndex(listenerId);
    if (listenerIndex == listeners_.size()) return false;

    listeners_.erase(
        listeners_.begin() + static_cast<std::ptrdiff_t>(listenerIndex));
    return true;
}

std::size_t GameStateManager::findListenerIndex(
    ListenerId listenerId) const {
    for (std::size_t index = 0; index < listeners_.size(); ++index) {
        if (listeners_[index].id == listenerId) return index;
    }
    return listeners_.size();
}
