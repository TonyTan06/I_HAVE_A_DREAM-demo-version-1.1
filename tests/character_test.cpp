#include "character.h"

#include <gtest/gtest.h>

TEST(CharacterTest, StartsAliveAndUsesDefaultGravityScale) {
    Character character("Test Character");

    EXPECT_TRUE(character.isAlive());
    EXPECT_FLOAT_EQ(character.getGravityScale(), 1.0F);
    EXPECT_FLOAT_EQ(character.getHitboxWidth(), 32.0F);
    EXPECT_FLOAT_EQ(character.getHitboxHeight(), 48.0F);
}

TEST(CharacterTest, RejectsNegativeGravityScale) {
    Character character("Test Character");

    character.setGravityScale(2.0F);
    character.setGravityScale(-1.0F);

    EXPECT_FLOAT_EQ(character.getGravityScale(), 2.0F);
}

TEST(CharacterTest, ProvidesAnOverridableRangedAttackFramework) {
    Character character("Test Character");

    character.rangedAttack();

    SUCCEED();
}

TEST(CharacterTest, CanLandAtAnElevatedPlatformHeight) {
    Character character("Character");

    character.landAtHeight(60.0F);
    EXPECT_TRUE(character.isGrounded());
    EXPECT_FLOAT_EQ(character.getY(), 60.0F);

    character.beginFalling();
    EXPECT_FALSE(character.isGrounded());
}

TEST(CharacterTest, SwapsPositionAndGroundedPhysicsState) {
    Character first("First");
    Character second("Second");
    first.setPosition(10.0F, 60.0F);
    first.landAtHeight(60.0F);
    second.setPosition(200.0F, 20.0F);
    second.beginFalling();

    first.swapPositionAndPhysics(second);

    EXPECT_FLOAT_EQ(first.getX(), 200.0F);
    EXPECT_FLOAT_EQ(first.getY(), 20.0F);
    EXPECT_FALSE(first.isGrounded());
    EXPECT_FLOAT_EQ(second.getX(), 10.0F);
    EXPECT_FLOAT_EQ(second.getY(), 60.0F);
    EXPECT_TRUE(second.isGrounded());
}
