#include "player_sprite_renderer.h"

#include <gtest/gtest.h>

TEST(PlayerSpriteRendererTest, UsesTopFrameForIdleAndBottomFrameForWalking) {
    const Rectangle idle = PlayerSpriteRenderer::makeSourceRectangle(
        96.0F, 256.0F, false, false);
    const Rectangle walking = PlayerSpriteRenderer::makeSourceRectangle(
        96.0F, 256.0F, true, false);

    EXPECT_FLOAT_EQ(idle.y, 0.0F);
    EXPECT_FLOAT_EQ(idle.height, 128.0F);
    EXPECT_FLOAT_EQ(walking.y, 128.0F);
    EXPECT_FLOAT_EQ(walking.height, 128.0F);
}

TEST(PlayerSpriteRendererTest, FlipsNativeLeftFacingFrameWhenPlayerFacesRight) {
    const Rectangle facingLeft = PlayerSpriteRenderer::makeSourceRectangle(
        96.0F, 256.0F, false, false);
    const Rectangle facingRight = PlayerSpriteRenderer::makeSourceRectangle(
        96.0F, 256.0F, false, true);

    EXPECT_FLOAT_EQ(facingLeft.width, 96.0F);
    EXPECT_FLOAT_EQ(facingRight.width, -96.0F);
}
