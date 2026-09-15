#include "engine/game_engine.h"
#include "engine/scene.h"
#include "game_state_listener_helpers.h"

#include <gtest/gtest.h>

#include <memory>

namespace {

class StubScene final : public Scene {
public:
    bool load() override {
        return true;
    }

    void update(float deltaTime) override {
        (void)deltaTime;
    }

    void draw() const override {
    }
};

} // namespace

TEST(GameEngineTest, OwnsInjectedSceneBeforeRun) {
    GameEngine engine(std::make_unique<StubScene>());

    EXPECT_TRUE(engine.hasScene());
    EXPECT_FALSE(engine.isRunning());
    EXPECT_EQ(engine.getGameState(), GameState::Booting);
}

TEST(GameEngineTest, AcceptsMissingSceneWithoutOpeningWindow) {
    GameEngine engine(nullptr);
    GameStateChangedEvent receivedEvent{
        GameState::Booting, GameState::Booting};
    int notificationCount = 0;
    const GameStateManager::ListenerId listenerId =
        engine.getGameStateManager().addStateChangedListener(
            StateChangeRecorder{receivedEvent, notificationCount});

    EXPECT_FALSE(engine.hasScene());
    EXPECT_NE(engine.run(), 0);
    EXPECT_EQ(engine.getGameState(), GameState::Exiting);
    EXPECT_EQ(notificationCount, 1);
    EXPECT_EQ(receivedEvent.previousState, GameState::Booting);
    EXPECT_EQ(receivedEvent.currentState, GameState::Exiting);
    EXPECT_TRUE(
        engine.getGameStateManager().removeStateChangedListener(listenerId));
}
