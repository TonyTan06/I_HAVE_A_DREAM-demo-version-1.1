#include "entities/player.h"

#include <gtest/gtest.h>

TEST(PlayerTest, StartsWithCurrentBaseHealthAndPosition) {
    Player player("Player");
    EXPECT_FLOAT_EQ(player.getX(), 140.0F);
    EXPECT_EQ(player.getHealth(), 1);
    EXPECT_EQ(player.getFaction(), Faction::Friendly);
}

TEST(PlayerTest, ReviveRestoresOneHealthAndClearsPendingAction) {
    Player player("Player");
    ASSERT_TRUE(player.rangedAttack());
    ASSERT_TRUE(player.tryBeginMeleeAttack());
    ASSERT_TRUE(player.startDodge(true));
    player.takeDamage();
    ASSERT_FALSE(player.isAlive());

    player.revive();
    EXPECT_TRUE(player.isAlive());
    EXPECT_EQ(player.getHealth(), 1);
    EXPECT_FALSE(player.consumeRangedAttackRequest());
    EXPECT_FALSE(player.isDodging());
    EXPECT_FALSE(player.isDodgeCoolingDown());
    EXPECT_TRUE(player.isMeleeAttackReady());
    EXPECT_TRUE(player.isRangedAttackReady());
}

TEST(PlayerTest, DefensePreventsBothAttackTypes) {
    Player player("Player");
    player.setDefending(true);
    EXPECT_FALSE(player.tryBeginMeleeAttack());
    EXPECT_FALSE(player.rangedAttack());
    EXPECT_FALSE(player.consumeRangedAttackRequest());
}

TEST(PlayerTest, RangedRequestCanBeConsumedOnce) {
    Player player("Player");
    ASSERT_TRUE(player.rangedAttack());
    EXPECT_TRUE(player.consumeRangedAttackRequest());
    EXPECT_FALSE(player.consumeRangedAttackRequest());
    EXPECT_FALSE(player.rangedAttack());
    player.update(1.0F, 980.0F);
    EXPECT_TRUE(player.rangedAttack());
}
