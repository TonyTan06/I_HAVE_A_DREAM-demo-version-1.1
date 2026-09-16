#include "rendering/character_renderer.h"

#include "entities/enemy.h"
#include "entities/player.h"
#include "entities/player_shadow.h"
#include "rendering/player_sprite_renderer.h"

#include <algorithm>

void CharacterRenderer::drawPlayer(
    const Player& player,
    float groundY,
    const PlayerSpriteRenderer& playerSpriteRenderer,
    bool attackVisible,
    float attackRange,
    float defenseRange) const {
    playerSpriteRenderer.draw(player, groundY, WHITE);
    const float playerHeight = player.getHitboxHeight();
    const float playerTop = groundY - playerHeight - player.getY();
    if (player.isDodgeCoolingDown()) {
        drawBackVerticalCooldownBar(
            player,
            player.isFacingRight(),
            playerTop,
            player.getDodgeCooldownProgress());
    }

    drawPlayerBlade(
        player,
        groundY,
        attackRange,
        defenseRange,
        attackVisible);
}

void CharacterRenderer::drawShadow(
    const PlayerShadow& shadow,
    float groundY,
    bool attackVisible,
    float attackRange,
    float defenseRange) const {
    drawPlayerBlade(
        shadow, groundY, attackRange, defenseRange, attackVisible);
}

void CharacterRenderer::drawEnemy(
    const Enemy& enemy,
    float groundY,
    Color eyeColor,
    bool attackVisible,
    float attackRange) const {
    const float enemyWidth = enemy.getHitboxWidth();
    const float enemyHeight = enemy.getHitboxHeight();
    const float enemyTop = groundY - enemyHeight - enemy.getY();
    const int barX = static_cast<int>(enemy.getX());
    const int barY = static_cast<int>(enemyTop - 10.0F);

    DrawRectangle(
        static_cast<int>(enemy.getX()),
        static_cast<int>(enemyTop),
        static_cast<int>(enemyWidth),
        static_cast<int>(enemyHeight),
        RED);
    DrawRectangle(barX, barY, static_cast<int>(enemyWidth), 5, DARKGRAY);

    const float eyeX = enemy.isFacingRight() ? enemy.getX() + enemyWidth - 8.0F : enemy.getX() + 8.0F;
    DrawCircle(
        static_cast<int>(eyeX),
        static_cast<int>(enemyTop + 14.0F),
        3.0F,
        eyeColor);
    if (attackVisible) {
        drawEnemyBlade(enemy, groundY, attackRange);
    }
}

void CharacterRenderer::drawBackVerticalCooldownBar(
    const Player& character,
    bool facingRight,
    float characterTop,
    float cooldownProgress) {
    const float clampedProgress = std::clamp(cooldownProgress, 0.0F, 1.0F);
    const int barX = static_cast<int>(facingRight ? character.getX() - 6.0F : character.getX() + character.getHitboxWidth() + 2.0F);
    const int barY = static_cast<int>(characterTop);
    const int barHeight = static_cast<int>(character.getHitboxHeight());
    const int progressHeight =
        static_cast<int>(barHeight * clampedProgress);

    DrawRectangle(barX, barY, 4, barHeight, DARKGRAY);
    DrawRectangle(
        barX,
        barY + barHeight - progressHeight,
        4,
        progressHeight,
        GREEN);
}

void CharacterRenderer::drawPlayerBlade(
    const Player& character,
    float groundY,
    float attackRange,
    float defenseRange,
    bool attackVisible) {
    if (!character.isDefending() && !attackVisible) return;

    const float characterHeight = character.getHitboxHeight();
    const float characterTop =
        groundY - characterHeight - character.getY();
    const float bladeBaseX = character.isFacingRight() ? character.getX() + character.getHitboxWidth() : character.getX();
    const Vector2 bladeTop{bladeBaseX, characterTop};
    const Vector2 bladeBottom{
        bladeBaseX, characterTop + characterHeight};
    const float bladeLength =
        character.isDefending() ? defenseRange : attackRange;
    const Vector2 bladeTip{
        character.isFacingRight() ? bladeBaseX + bladeLength : bladeBaseX - bladeLength,
        characterTop + characterHeight / 2.0F};
    const Color bladeColor = character.isDefending() ? Color{255, 222, 173, 255} : WHITE;

    if (character.isFacingRight()) {
        DrawTriangle(bladeTop, bladeBottom, bladeTip, bladeColor);
    } else {
        DrawTriangle(bladeBottom, bladeTop, bladeTip, bladeColor);
    }
}

void CharacterRenderer::drawEnemyBlade(
    const Enemy& enemy, float groundY, float attackRange) {
    const float enemyHeight = enemy.getHitboxHeight();
    const float enemyTop = groundY - enemyHeight - enemy.getY();
    const float bladeBaseX = enemy.isFacingRight() ? enemy.getX() + enemy.getHitboxWidth() : enemy.getX();
    const Vector2 bladeTop{bladeBaseX, enemyTop};
    const Vector2 bladeBottom{bladeBaseX, enemyTop + enemyHeight};
    const Vector2 bladeTip{
        enemy.isFacingRight() ? bladeBaseX + attackRange : bladeBaseX - attackRange,
        enemyTop + enemyHeight / 2.0F};
    const Color bladeColor{255, 182, 193, 255};

    if (enemy.isFacingRight()) {
        DrawTriangle(bladeTop, bladeBottom, bladeTip, bladeColor);
    } else {
        DrawTriangle(bladeBottom, bladeTop, bladeTip, bladeColor);
    }
}
