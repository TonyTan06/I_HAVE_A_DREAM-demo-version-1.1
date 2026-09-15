#include "entities/character.h"

#include <gtest/gtest.h>

TEST(CharacterTest, GravityAndLandingUseWorldGravity) {
    Character character("Character");
    character.setPosition(0.0F, 100.0F);
    character.beginFalling();

    character.update(0.1F, 980.0F);
    EXPECT_NEAR(character.getY(), 90.2F, 0.001F);
    EXPECT_FALSE(character.isGrounded());

    character.landAtHeight(60.0F);
    EXPECT_FLOAT_EQ(character.getY(), 60.0F);
    EXPECT_TRUE(character.isGrounded());
}

TEST(CharacterTest, AttackCooldownsAreIndependent) {
    Character character("Character");
    EXPECT_TRUE(character.tryBeginMeleeAttack());
    EXPECT_FALSE(character.tryBeginMeleeAttack());
    EXPECT_TRUE(character.tryBeginRangedAttack());

    character.update(1.0F, 980.0F);
    EXPECT_TRUE(character.isMeleeAttackReady());
    EXPECT_TRUE(character.isRangedAttackReady());
}

TEST(CharacterTest, DamageKillsAndDeadCharacterCannotAttack) {
    Character character("Character");
    character.takeDamage();
    EXPECT_FALSE(character.isAlive());
    EXPECT_FALSE(character.tryBeginMeleeAttack());
}

TEST(CharacterTest, NegativeGravityScaleIsIgnored) {
    Character character("Character");
    character.setGravityScale(0.5F);
    character.setGravityScale(-1.0F);
    EXPECT_FLOAT_EQ(character.getGravityScale(), 0.5F);
}
