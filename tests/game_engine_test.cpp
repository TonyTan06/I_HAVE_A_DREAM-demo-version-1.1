#include "engine/game_engine.h"
#include "engine/scene.h"

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
}

TEST(GameEngineTest, AcceptsMissingSceneWithoutOpeningWindow) {
    GameEngine engine(nullptr);

    EXPECT_FALSE(engine.hasScene());
    EXPECT_NE(engine.run(), 0);
}
