#include "world/game_world.h"

#include <gtest/gtest.h>

TEST(GameWorldTest, EmptyEnemyListSupportsPlayerPhysics) {
    GameWorld world;
    EXPECT_TRUE(world.getEnemies().empty());
    PlayerInputState input;
    input.jumpPressed = true;

    world.update(input, 0.1F);
    EXPECT_NEAR(world.getPlayer().getY(), 26.2F, 0.001F);
    EXPECT_FLOAT_EQ(world.getGroundPlatform().y, 360.0F);
}

TEST(GameWorldTest, EmptyEnemyListStillAllowsPlayerAttacks) {
    GameWorld world;
    PlayerInputState input;
    input.meleeAttackPressed = true;
    input.rangedAttackPressed = true;

    const GameWorldFrameResult result = world.update(input, 0.0F);
    EXPECT_TRUE(result.playerMeleeAttackPerformed);
    EXPECT_FALSE(result.damage.occurred);
    EXPECT_EQ(world.getProjectileSystem().getProjectileCount(), 1U);
}

TEST(GameWorldTest, DefeatedPlayerRespawnsAtInitialPoint) {
    GameWorld world;
    world.getPlayer().setPosition(440.0F, 0.0F);
    world.update(PlayerInputState{}, 0.0F);
    ASSERT_NE(world.getShadow(), nullptr);
    world.getProjectileSystem().spawn(ProjectileSystem::SpawnInfo{
        1000.0F, 300.0F, 1.0F, 100.0F, 100.0F, 4.0F,
        1.0F, Faction::Friendly, ORANGE, 0});
    ASSERT_EQ(world.getProjectileSystem().getProjectileCount(), 1U);
    world.getPlayer().setPosition(500.0F, 0.0F);
    world.getPlayer().takeDamage();
    ASSERT_FALSE(world.getPlayer().isAlive());

    const GameWorldFrameResult result =
        world.update(PlayerInputState{}, 0.0F);
    EXPECT_TRUE(world.getPlayer().isAlive());
    EXPECT_EQ(world.getPlayer().getHealth(), 1);
    EXPECT_FLOAT_EQ(world.getPlayer().getX(), 140.0F);
    EXPECT_EQ(world.getProjectileSystem().getProjectileCount(), 0U);
    EXPECT_EQ(world.getShadow(), nullptr);
    ASSERT_EQ(result.events.size(), 2U);
    EXPECT_EQ(result.events[0], GameEvent::PlayerDied);
    EXPECT_EQ(result.events[1], GameEvent::PlayerRespawned);
}

TEST(GameWorldTest, FatalProjectilePreventsMeleeBeforeRespawn) {
    GameWorld world;
    world.getProjectileSystem().spawn(ProjectileSystem::SpawnInfo{
        160.0F, 300.0F, 1.0F, 100.0F, 100.0F, 4.0F,
        1.0F, Faction::Enemy, RED, 0});
    PlayerInputState input;
    input.meleeAttackPressed = true;

    const GameWorldFrameResult result = world.update(input, 0.0F);
    EXPECT_FALSE(result.playerMeleeAttackPerformed);
    EXPECT_TRUE(result.damage.occurred);
    EXPECT_TRUE(world.getPlayer().isAlive());
    ASSERT_EQ(result.events.size(), 2U);
    EXPECT_EQ(result.events[0], GameEvent::PlayerDied);
    EXPECT_EQ(result.events[1], GameEvent::PlayerRespawned);
}

TEST(GameWorldTest, NormalFrameProducesNoGlobalEvents) {
    GameWorld world;

    const GameWorldFrameResult result =
        world.update(PlayerInputState{}, 0.0F);

    EXPECT_TRUE(result.events.empty());
}
