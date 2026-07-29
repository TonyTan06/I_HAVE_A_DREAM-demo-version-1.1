#include "rendering/character_renderer.h"

#include "entities/enemy.h"
#include "entities/player.h"
#include "entities/player_shadow.h"
#include "rendering/player_sprite_renderer.h"

#include <algorithm>

void CharacterRenderer::draw(
    const Player& player,
    const Enemy& meleeEnemy,
    const Enemy& rangedEnemy,
    const PlayerShadow* shadow,
    float groundY,
    const PlayerSpriteRenderer& playerSpriteRenderer,
    const CharacterEffectView& effects) const {
    drawEnemy(meleeEnemy, groundY, BLUE);
    drawEnemy(rangedEnemy, groundY, GREEN);

    playerSpriteRenderer.draw(player, groundY, WHITE);
    const float playerWidth = player.getHitboxWidth();
    const float playerHeight = player.getHitboxHeight();
    const float playerTop = groundY - playerHeight - player.getY();
    if (player.isDodgeCoolingDown()) {
        drawBackVerticalCooldownBar(
            player,
            player.isFacingRight(),
            playerTop,
            player.getDodgeCooldownProgress());
    }

    const float playerHealthRatio = player.getHealth() / player.getMaxHealth();
    DrawRectangle(
        static_cast<int>(player.getX()),
        static_cast<int>(playerTop - 10.0F),
        static_cast<int>(playerWidth),
        5,
        DARKGRAY);
    DrawRectangle(
        static_cast<int>(player.getX()),
        static_cast<int>(playerTop - 10.0F),
        static_cast<int>(
            playerWidth * std::clamp(playerHealthRatio, 0.0F, 1.0F)),
        5,
        GREEN);
    const float playerEyeX = player.isFacingRight() ? player.getX() + playerWidth - 8.0F : player.getX() + 8.0F;
    DrawCircle(
        static_cast<int>(playerEyeX),
        static_cast<int>(playerTop + 14.0F),
        3.0F,
        BLACK);

    if (shadow != nullptr && shadow->isPositionSwapCoolingDown()) {
        const float shadowTop =
            groundY - shadow->getHitboxHeight() - shadow->getY();
        drawBackVerticalCooldownBar(
            *shadow,
            shadow->isFacingRight(),
            shadowTop,
            shadow->getPositionSwapCooldownProgress());
    }

    drawPlayerBlade(
        player,
        groundY,
        effects.attackRange,
        effects.defenseRange,
        effects.playerAttackVisible);
    if (shadow != nullptr) {
        drawPlayerBlade(
            *shadow,
            groundY,
            effects.attackRange,
            effects.defenseRange,
            effects.shadowAttackVisible);
    }
    if (effects.meleeEnemyAttackVisible) {
        drawMeleeEnemyBlade(meleeEnemy, groundY, effects.attackRange);
    }
}

void CharacterRenderer::drawEnemy(
    const Enemy& enemy, float groundY, Color eyeColor) {
    const float enemyWidth = enemy.getHitboxWidth();
    const float enemyHeight = enemy.getHitboxHeight();
    const float enemyTop = groundY - enemyHeight - enemy.getY();
    const int barX = static_cast<int>(enemy.getX());
    const int barY = static_cast<int>(enemyTop - 10.0F);
    const bool respawning = !enemy.isAlive() || enemy.isRespawning();
    const Color bodyColor = respawning ? Color{235, 170, 170, 255} : Color{190, 90, 90, 255};
    const float barProgress = respawning ? 1.0F - enemy.getRespawnProgress() : enemy.getHealth() / enemy.getMaxHealth();

    DrawRectangle(
        static_cast<int>(enemy.getX()),
        static_cast<int>(enemyTop),
        static_cast<int>(enemyWidth),
        static_cast<int>(enemyHeight),
        bodyColor);
    DrawRectangle(barX, barY, static_cast<int>(enemyWidth), 5, DARKGRAY);
    DrawRectangle(
        barX,
        barY,
        static_cast<int>(
            enemyWidth * std::clamp(barProgress, 0.0F, 1.0F)),
        5,
        respawning ? YELLOW : RED);

    const float eyeX = enemy.isFacingRight() ? enemy.getX() + enemyWidth - 8.0F : enemy.getX() + 8.0F;
    DrawCircle(
        static_cast<int>(eyeX),
        static_cast<int>(enemyTop + 14.0F),
        3.0F,
        eyeColor);
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

void CharacterRenderer::drawMeleeEnemyBlade(
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
