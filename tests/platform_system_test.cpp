#include "character.h"
#include "platform_system.h"

#include <gtest/gtest.h>

TEST(PlatformSystemTest, LandsFallingCharacterOnPlatformTop) {
    PlatformSystem platforms;
    platforms.addPlatform(Rectangle{100.0F, 300.0F, 100.0F, 5.0F});
    Character character("Character");
    character.setPosition(120.0F, 50.0F);
    character.beginFalling();

    platforms.resolveCharacterLanding(character, 70.0F, 360.0F);

    EXPECT_TRUE(character.isGrounded());
    EXPECT_FLOAT_EQ(character.getY(), 60.0F);
}

TEST(PlatformSystemTest, CharacterFallsAfterLeavingPlatform) {
    PlatformSystem platforms;
    platforms.addPlatform(Rectangle{100.0F, 300.0F, 100.0F, 5.0F});
    Character character("Character");
    character.setPosition(220.0F, 60.0F);
    character.landAtHeight(60.0F);

    platforms.updateCharacterSupport(character, 360.0F);

    EXPECT_FALSE(character.isGrounded());
}

TEST(PlatformSystemTest, IgnoresInvalidPlatformSize) {
    PlatformSystem platforms;

    platforms.addPlatform(Rectangle{0.0F, 0.0F, 0.0F, 5.0F});

    EXPECT_TRUE(platforms.getPlatforms().empty());
}
