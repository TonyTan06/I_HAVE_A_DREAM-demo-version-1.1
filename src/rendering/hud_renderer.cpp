#include "rendering/hud_renderer.h"

#include "entities/player.h"
#include "raylib.h"

void HudRenderer::draw(
    const Player& player, const DamageTextView& damageText) const {
    if (damageText.visible) {
        DrawText(
            TextFormat("%.0f", damageText.damage),
            static_cast<int>(damageText.x),
            static_cast<int>(damageText.y),
            20,
            YELLOW);
    }

    DrawText(
        "A/D or arrow keys: move    Space: jump    J: melee    K: ranged    "
        "U: defend    L: dodge    1: sync shadow    2: swap",
        20,
        20,
        20,
        RAYWHITE);
    DrawText(
        TextFormat("Level: %d", player.getLevel()),
        20,
        50,
        20,
        RAYWHITE);
    DrawText(
        TextFormat(
            "Exp: %d / %d",
            player.getExperience(),
            player.getExperienceThreshold()),
        20,
        76,
        20,
        RAYWHITE);
}
