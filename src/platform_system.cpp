#include "platform_system.h"

#include "character.h"

#include <algorithm>
#include <cmath>

void PlatformSystem::addPlatform(const Rectangle& platform) {
    if (platform.width <= 0.0F || platform.height <= 0.0F) return;

    platforms_.push_back(platform);
}

void PlatformSystem::draw(Color color) const {
    for (const Rectangle& platform : platforms_) {
        DrawRectangleRec(platform, color);
    }
}

void PlatformSystem::updateCharacterSupport(
    Character& character, float groundY) const {
    if (!character.isGrounded() || character.getY() <= 0.0F) return;

    const bool stillSupported = std::any_of(
        platforms_.begin(), platforms_.end(), [&](const Rectangle& platform) {
            const float platformHeight = groundY - platform.y;
            return std::abs(character.getY() - platformHeight) <= 0.01F &&
                overlapsHorizontally(character, platform);
        });
    if (!stillSupported) {
        character.beginFalling();
    }
}

void PlatformSystem::resolveCharacterLanding(
    Character& character, float previousHeight,
    float groundY) const {
    if (character.isGrounded()) return;

    float landingHeight = -1.0F; // 本帧穿过的最高平台顶面相对地面的高度
    for (const Rectangle& platform : platforms_) {
        if (!overlapsHorizontally(character, platform)) continue;

        const float platformHeight = groundY - platform.y;
        const bool crossedTopWhileFalling =
            previousHeight >= platformHeight && character.getY() <= platformHeight;
        if (crossedTopWhileFalling) {
            landingHeight = std::max(landingHeight, platformHeight);
        }
    }

    if (landingHeight >= 0.0F) {
        character.landAtHeight(landingHeight);
    }
}

const std::vector<Rectangle>& PlatformSystem::getPlatforms() const {
    return platforms_;
}

bool PlatformSystem::overlapsHorizontally(
    const Character& character, const Rectangle& platform) {
    const float characterLeft = character.getX();
    const float characterRight = characterLeft + character.getHitboxWidth();
    const float platformRight = platform.x + platform.width;
    return characterRight > platform.x && characterLeft < platformRight;
}
