#include "scenes/game_scene.h"

#include <gtest/gtest.h>

TEST(GameSceneTest, CanBeCreatedWithoutOpeningAWindow) {
    GameScene scene;

    SUCCEED();
}
