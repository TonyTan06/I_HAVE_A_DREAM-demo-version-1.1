#include "engine/game_event.h"

#include <gtest/gtest.h>

TEST(GameEventTest, DefinesDistinctGlobalEvents) {
    EXPECT_NE(GameEvent::PlayerDied, GameEvent::PlayerRespawned);
    EXPECT_NE(GameEvent::LevelStarted, GameEvent::LevelCompleted);
    EXPECT_NE(GameEvent::BossStarted, GameEvent::BossDefeated);
}
