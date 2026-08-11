#include "entities/player.h"
#include "entities/player_shadow.h"
#include "input/player_controller.h"

#include <gtest/gtest.h>

TEST(PlayerShadowTest, CopiesPlayerStatsAndRecordedPosition) {
    Player player("Player");
    player.jump();
    PlayerShadow shadow(player, 25.0F, 40.0F);

    EXPECT_FLOAT_EQ(shadow.getX(), 25.0F);
    EXPECT_FLOAT_EQ(shadow.getY(), 40.0F);
    EXPECT_FLOAT_EQ(shadow.getMoveSpeed(), player.getMoveSpeed());
    EXPECT_FLOAT_EQ(shadow.getPhysicalDefense(), player.getPhysicalDefense());
    EXPECT_FLOAT_EQ(shadow.getMagicalDefense(), player.getMagicalDefense());
    EXPECT_FLOAT_EQ(shadow.getAttackDamage(), player.getAttackDamage());
    EXPECT_FLOAT_EQ(shadow.getAttacksPerSecond(), player.getAttacksPerSecond());
    EXPECT_FLOAT_EQ(shadow.getShield(), player.getShield());
    EXPECT_FLOAT_EQ(shadow.getJumpInitialVelocity(), player.getJumpInitialVelocity());
    EXPECT_FLOAT_EQ(shadow.getGravityScale(), player.getGravityScale());
    EXPECT_EQ(shadow.getJumpCount(), player.getJumpCount());
    EXPECT_EQ(shadow.getMaxJumpCount(), player.getMaxJumpCount());
    EXPECT_EQ(shadow.getFaction(), Faction::Friendly);
}

TEST(PlayerShadowTest, IgnoresDamageBecauseItHasNoHealthMechanic) {
    Player player("Player");
    PlayerShadow shadow(player, 0.0F, 0.0F);

    player.attack(shadow);

    EXPECT_FLOAT_EQ(shadow.getHealth(), shadow.getMaxHealth());
}

TEST(PlayerShadowTest, IsGroundedOnlyWhenCreatedOnTheGround) {
    Player player("Player");
    PlayerShadow groundedShadow(player, 0.0F, 0.0F);
    PlayerShadow airborneShadow(player, 0.0F, 20.0F);

    EXPECT_TRUE(groundedShadow.isGrounded());
    EXPECT_FALSE(airborneShadow.isGrounded());
}

TEST(PlayerShadowTest, SupportsTheSameControllableActionsAsPlayer) {
    Player player("Player");
    PlayerShadow shadow(player, 0.0F, 0.0F);

    shadow.moveRight(0.5F);
    shadow.jump();
    shadow.rangedAttack();

    EXPECT_FLOAT_EQ(shadow.getX(), player.getMoveSpeed() * 0.5F);
    EXPECT_FALSE(shadow.isGrounded());
    EXPECT_TRUE(shadow.consumeRangedAttackRequest());
}

TEST(PlayerShadowTest, OwnsAndExecutesSynchronizedInputSkill) {
    Player player("Player");
    PlayerShadow shadow(player, 0.0F, 0.0F);
    PlayerInputState input;
    input.moveRightHeld = true;
    input.meleeAttackPressed = true;

    shadow.setActiveSkill(ShadowSkill::SynchronizePlayerActions);
    const bool meleeRequested = shadow.applyInput(
        input, HorizontalInputDirection::Right, 0.5F);

    EXPECT_EQ(shadow.getActiveSkill(), ShadowSkill::SynchronizePlayerActions);
    EXPECT_TRUE(meleeRequested);
    EXPECT_FLOAT_EQ(shadow.getX(), shadow.getMoveSpeed() * 0.5F);
    EXPECT_EQ(shadow.getFaction(), Faction::Friendly);
}

TEST(PlayerShadowTest, SkillTwoSwapsPhysicsAndReturnsToSkillOne) {
    Player player("Player");
    player.setPosition(300.0F, 40.0F);
    player.beginFalling();
    PlayerShadow shadow(player, 100.0F, 0.0F);
    shadow.land();

    ASSERT_TRUE(shadow.tryUsePositionSwap(player));

    EXPECT_FLOAT_EQ(player.getX(), 100.0F);
    EXPECT_FLOAT_EQ(player.getY(), 0.0F);
    EXPECT_TRUE(player.isGrounded());
    EXPECT_FLOAT_EQ(shadow.getX(), 300.0F);
    EXPECT_FLOAT_EQ(shadow.getY(), 40.0F);
    EXPECT_FALSE(shadow.isGrounded());
    EXPECT_EQ(shadow.getActiveSkill(), ShadowSkill::SynchronizePlayerActions);
    EXPECT_TRUE(shadow.isPositionSwapCoolingDown());
}

TEST(PlayerShadowTest, SkillTwoHasTenSecondCooldown) {
    Player player("Player");
    PlayerShadow shadow(player, 0.0F, 0.0F);

    ASSERT_TRUE(shadow.tryUsePositionSwap(player));
    EXPECT_FALSE(shadow.tryUsePositionSwap(player));
    EXPECT_FLOAT_EQ(shadow.getPositionSwapCooldownProgress(), 0.0F);

    shadow.update(5.0F);
    EXPECT_TRUE(shadow.isPositionSwapCoolingDown());
    EXPECT_FLOAT_EQ(shadow.getPositionSwapCooldownProgress(), 0.5F);

    shadow.update(5.0F);
    EXPECT_FALSE(shadow.isPositionSwapCoolingDown());
    EXPECT_TRUE(shadow.tryUsePositionSwap(player));
}
