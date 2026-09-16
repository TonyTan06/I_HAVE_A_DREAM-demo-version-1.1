#include "engine/game_state.h"
#include "game_state_listener_helpers.h"

#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

namespace {

struct PauseWhenPlayingListener {
    GameStateManager& stateManager;

    void operator()(const GameStateChangedEvent& event) const {
        if (event.currentState == GameState::Playing) {
            stateManager.transitionTo(GameState::Paused);
        }
    }
};

struct StateSequenceRecorder {
    const GameStateManager& stateManager;
    std::vector<GameState>& receivedEventStates;
    std::vector<GameState>& observedCurrentStates;

    void operator()(const GameStateChangedEvent& event) const {
        receivedEventStates.push_back(event.currentState);
        observedCurrentStates.push_back(stateManager.getCurrentState());
    }
};

struct RemoveListenerOnStateChange {
    const GameStateManager& stateManager;
    const GameStateManager::ListenerId& listenerId;

    void operator()(const GameStateChangedEvent&) const {
        stateManager.removeStateChangedListener(listenerId);
    }
};

} // namespace

TEST(GameStateManagerTest, KeepsTheExplicitInitialState) {
    const GameStateManager stateManager(GameState::MainMenu);

    EXPECT_EQ(stateManager.getCurrentState(), GameState::MainMenu);
}

TEST(GameStateManagerTest, TransitionUpdatesStateAndSendsPreviousAndCurrentState) {
    GameStateManager stateManager(GameState::MainMenu);
    GameStateChangedEvent receivedEvent{
        GameState::Booting, GameState::Booting};
    int notificationCount = 0;
    stateManager.addStateChangedListener(
        StateChangeRecorder{receivedEvent, notificationCount});

    EXPECT_TRUE(stateManager.transitionTo(GameState::Playing));
    EXPECT_EQ(stateManager.getCurrentState(), GameState::Playing);
    EXPECT_EQ(notificationCount, 1);
    EXPECT_EQ(receivedEvent.previousState, GameState::MainMenu);
    EXPECT_EQ(receivedEvent.currentState, GameState::Playing);
}

TEST(GameStateManagerTest, SameStateDoesNotSendADuplicateEvent) {
    GameStateManager stateManager(GameState::Playing);
    int notificationCount = 0;
    stateManager.addStateChangedListener(
        StateChangeCounter{notificationCount});

    EXPECT_FALSE(stateManager.transitionTo(GameState::Playing));
    EXPECT_EQ(notificationCount, 0);
}

TEST(GameStateManagerTest, NotifiesEveryRegisteredListener) {
    GameStateManager stateManager(GameState::Loading);
    int firstListenerCount = 0;
    int secondListenerCount = 0;
    stateManager.addStateChangedListener(
        StateChangeCounter{firstListenerCount});
    stateManager.addStateChangedListener(
        StateChangeCounter{secondListenerCount});

    stateManager.transitionTo(GameState::Playing);

    EXPECT_EQ(firstListenerCount, 1);
    EXPECT_EQ(secondListenerCount, 1);
}

TEST(GameStateManagerTest, RemovedListenerDoesNotReceiveLaterEvents) {
    GameStateManager stateManager(GameState::Playing);
    int notificationCount = 0;
    const GameStateManager::ListenerId listenerId =
        stateManager.addStateChangedListener(
            StateChangeCounter{notificationCount});

    EXPECT_TRUE(stateManager.removeStateChangedListener(listenerId));
    EXPECT_FALSE(stateManager.removeStateChangedListener(listenerId));
    stateManager.transitionTo(GameState::Paused);

    EXPECT_EQ(notificationCount, 0);
}

TEST(GameStateManagerTest, QueuesNestedTransitionsUntilCurrentEventFinishes) {
    GameStateManager stateManager(GameState::MainMenu);
    std::vector<GameState> receivedEventStates;
    std::vector<GameState> observedCurrentStates;
    stateManager.addStateChangedListener(
        PauseWhenPlayingListener{stateManager});
    stateManager.addStateChangedListener(StateSequenceRecorder{
        stateManager, receivedEventStates, observedCurrentStates});

    EXPECT_TRUE(stateManager.transitionTo(GameState::Playing));

    ASSERT_EQ(receivedEventStates.size(), 2U);
    EXPECT_EQ(receivedEventStates[0], GameState::Playing);
    EXPECT_EQ(receivedEventStates[1], GameState::Paused);
    EXPECT_EQ(observedCurrentStates, receivedEventStates);
    EXPECT_EQ(stateManager.getCurrentState(), GameState::Paused);
}

TEST(GameStateManagerTest, RemovalDuringAnEventSkipsTheRemovedListener) {
    GameStateManager stateManager(GameState::Playing);
    GameStateManager::ListenerId removedListenerId = 0;
    int removedListenerCount = 0;
    stateManager.addStateChangedListener(
        RemoveListenerOnStateChange{stateManager, removedListenerId});
    removedListenerId = stateManager.addStateChangedListener(
        StateChangeCounter{removedListenerCount});

    stateManager.transitionTo(GameState::Paused);

    EXPECT_EQ(removedListenerCount, 0);
}

TEST(GameStateManagerTest, RejectsAnEmptyListener) {
    GameStateManager stateManager(GameState::Playing);

    EXPECT_THROW(
        stateManager.addStateChangedListener(
            GameStateManager::StateChangedListener{}),
        std::invalid_argument);
}
