#include "entities/player.h"
#include "systems/shadow_manager.h"

#include <gtest/gtest.h>

TEST(ShadowManagerTest, SpawnsAtRecordedPointAfterTravelDistance) {
    Player player("Player");
    ShadowManager shadows(player);
    player.setPosition(440.0F, 0.0F);

    shadows.update(player, 0.0F, 980.0F);
    ASSERT_TRUE(shadows.hasShadow());
    ASSERT_NE(shadows.getShadow(), nullptr);
    EXPECT_FLOAT_EQ(shadows.getShadow()->getX(), 140.0F);
}

TEST(ShadowManagerTest, RemovesShadowAfterTenSeconds) {
    Player player("Player");
    ShadowManager shadows(player);
    player.setPosition(440.0F, 0.0F);
    shadows.update(player, 0.0F, 980.0F);
    ASSERT_TRUE(shadows.hasShadow());

    shadows.update(player, 10.0F, 980.0F);
    EXPECT_FALSE(shadows.hasShadow());
}

TEST(ShadowManagerTest, ForwardsWorldGravityToShadow) {
    Player player("Player");
    ShadowManager shadows(player);
    player.setPosition(440.0F, 0.0F);
    shadows.update(player, 0.0F, 980.0F);
    ASSERT_NE(shadows.getShadow(), nullptr);
    shadows.getShadow()->setPosition(140.0F, 100.0F);
    shadows.getShadow()->beginFalling();

    shadows.update(player, 0.1F, 980.0F);
    EXPECT_NEAR(shadows.getShadow()->getY(), 90.2F, 0.001F);
}

TEST(ShadowManagerTest, ResetRemovesShadowAndStartsNewRecording) {
    Player player("Player");
    ShadowManager shadows(player);
    player.setPosition(440.0F, 0.0F);
    shadows.update(player, 0.0F, 980.0F);
    ASSERT_TRUE(shadows.hasShadow());

    shadows.reset(player);
    EXPECT_FALSE(shadows.hasShadow());
    player.setPosition(740.0F, 0.0F);
    shadows.update(player, 0.0F, 980.0F);
    ASSERT_NE(shadows.getShadow(), nullptr);
    EXPECT_FLOAT_EQ(shadows.getShadow()->getX(), 440.0F);
}
