#include "rendering/hud_renderer.h"

#include <gtest/gtest.h>

TEST(HudRendererTest, CanBeCreatedWithoutOpeningAWindow) {
    HudRenderer renderer;

    (void)renderer;
    SUCCEED();
}
