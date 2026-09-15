#include "rendering/hud_renderer.h"

#include "raylib.h"

void HudRenderer::draw(const DamageTextView& damageText) const {
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
        "U: defend    L: dodge       Esc: pause",
        20,
        20,
        20,
        RAYWHITE);
}
