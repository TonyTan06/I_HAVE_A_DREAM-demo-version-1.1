#include "entities/player_shadow.h"

#include <gtest/gtest.h>

TEST(PlayerShadowTest, CopiesMovementAttributesAtRecordedPoint) {
    Player player("Player");
    player.moveLeft(0.1F);
    PlayerShadow shadow(player, 30.0F, 60.0F);

    EXPECT_FLOAT_EQ(shadow.getX(), 30.0F);
    EXPECT_FLOAT_EQ(shadow.getY(), 60.0F);
    EXPECT_FLOAT_EQ(shadow.getMoveSpeed(), player.getMoveSpeed());
    EXPECT_FLOAT_EQ(shadow.getMeleeAttackRange(), player.getMeleeAttackRange());
    EXPECT_FLOAT_EQ(shadow.getRangedAttackRange(), player.getRangedAttackRange());
    EXPECT_FLOAT_EQ(shadow.getDefenseRange(), player.getDefenseRange());
    EXPECT_FALSE(shadow.isFacingRight());
    EXPECT_EQ(shadow.getFaction(), Faction::Neutral);
}

TEST(PlayerShadowTest, DirectDamageDoesNotRemoveShadow) {
    Player player("Player");
    PlayerShadow shadow(player, 30.0F, 0.0F);
    shadow.takeDamage();
    shadow.takeTrueDamage();
    EXPECT_TRUE(shadow.isAlive());
    EXPECT_EQ(shadow.getHealth(), 1);
}

TEST(PlayerShadowTest, UsesPassedWorldGravity) {
    Player player("Player");
    PlayerShadow shadow(player, 30.0F, 100.0F);
    shadow.update(0.1F, 980.0F);
    EXPECT_NEAR(shadow.getY(), 90.2F, 0.001F);
}
