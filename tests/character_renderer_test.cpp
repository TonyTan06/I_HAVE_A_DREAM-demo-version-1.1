#include "rendering/character_renderer.h"

#include <gtest/gtest.h>

TEST(CharacterRendererTest, CanBeCreatedWithoutOpeningAWindow) {
    CharacterRenderer renderer;

    (void)renderer;
    SUCCEED();
}
