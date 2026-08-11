#include "systems/shadow_manager.h"

#include "entities/player.h"

#include <gtest/gtest.h>

TEST(ShadowManagerTest, GeneratesShadowAtRecordedPointAfterThreeHundredPixels) {
    Player player("Player");
    ShadowManager manager(player);
    const float recordedX = player.getX();

    player.moveRight(1.25F);
    manager.update(player, 0.0F);

    ASSERT_TRUE(manager.hasShadow());
    ASSERT_NE(manager.getShadow(), nullptr);
    EXPECT_FLOAT_EQ(manager.getShadow()->getX(), recordedX);
}

TEST(ShadowManagerTest, RemovesShadowAfterTenSecondsAndStartsNewCycle) {
    Player player("Player");
    ShadowManager manager(player);
    player.moveRight(1.25F);
    manager.update(player, 0.0F);

    manager.update(player, 10.0F);

    EXPECT_FALSE(manager.hasShadow());
    EXPECT_EQ(manager.getShadow(), nullptr);
}

TEST(ShadowManagerTest, GeneratedShadowOwnsItsDefaultSkill) {
    Player player("Player");
    ShadowManager manager(player);
    player.moveRight(1.25F);
    manager.update(player, 0.0F);

    ASSERT_NE(manager.getShadow(), nullptr);
    EXPECT_EQ(manager.getShadow()->getActiveSkill(),
              ShadowSkill::SynchronizePlayerActions);
}

TEST(ShadowManagerTest, CountsOnlyThirtyThreePercentOfDodgeDistance) {
    Player player("Player");
    ShadowManager manager(player);

    player.startDodge(true);
    player.update(0.2F); // 闪避 240px，只累计 79.2px。
    manager.update(player, 0.2F);
    EXPECT_FALSE(manager.hasShadow());

    player.moveRight(0.9F); // 普通移动 216px，累计为 295.2px。
    player.update(0.0F);
    manager.update(player, 0.0F);
    EXPECT_FALSE(manager.hasShadow());

    player.moveRight(0.02F); // 再移动 4.8px，总累计达到 300px。
    player.update(0.0F);
    manager.update(player, 0.0F);
    EXPECT_TRUE(manager.hasShadow());
}
