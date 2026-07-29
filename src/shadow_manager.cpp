#include "shadow_manager.h"

#include "player.h"
#include "player_sprite_renderer.h"

#include <algorithm>
#include <cmath>

ShadowManager::ShadowManager(const Player& player)
    : shadow_(std::nullopt),
      recordedPlayerX_(player.getX()),
      recordedPlayerY_(player.getY()),
      previousPlayerX_(player.getX()),
      recordedFacingRight_(player.isFacingRight()),
      accumulatedHorizontalDistance_(0.0F),
      elapsedLifetime_(0.0F) {
}

void ShadowManager::update(const Player& player, float deltaTime) {
    // 用相邻两帧坐标差累计实际路程，因此玩家折返移动也会增加进度。
    const float frameHorizontalDistance = std::abs(player.getX() - previousPlayerX_);
    previousPlayerX_ = player.getX();

    if (shadow_.has_value()) {
        // 影子存在期间停止记录生成距离，只更新实体和十秒生命周期。
        shadow_->update(deltaTime);
        if (shadow_->getY() <= 0.0F) {
            shadow_->land();
        }

        elapsedLifetime_ += std::max(0.0F, deltaTime);
        if (elapsedLifetime_ >= LIFETIME) {
            // 销毁时记录玩家当前位置，下一轮从这里重新累计 300px。
            shadow_.reset();
            recordedPlayerX_ = player.getX();
            recordedPlayerY_ = player.getY();
            recordedFacingRight_ = player.isFacingRight();
            accumulatedHorizontalDistance_ = 0.0F;
            elapsedLifetime_ = 0.0F;
        }
        return;
    }

    // 普通移动按完整距离累计；由闪避产生的实际位移只累计 33%。
    // min 避免场景边界夹紧位置后，闪避理论距离大于角色实际位移。
    const float dodgeDistance =
        std::min(player.getLastDodgeDistance(), frameHorizontalDistance);
    const float normalDistance = frameHorizontalDistance - dodgeDistance;
    accumulatedHorizontalDistance_ +=
        normalDistance + dodgeDistance * DODGE_DISTANCE_MULTIPLIER;
    if (accumulatedHorizontalDistance_ >= SPAWN_DISTANCE) {
        // 实体生成在本轮开始时保存的记录点，而不是玩家当前坐标。
        shadow_.emplace(player, recordedPlayerX_, recordedPlayerY_);
        elapsedLifetime_ = 0.0F;
    }
}

void ShadowManager::resetPlayerTracking(const Player& player) {
    // 复活或传送后同步上一帧位置，避免把瞬移距离误算为移动进度。
    previousPlayerX_ = player.getX();
    recordedPlayerX_ = player.getX();
    recordedPlayerY_ = player.getY();
    recordedFacingRight_ = player.isFacingRight();
    accumulatedHorizontalDistance_ = 0.0F;
}

void ShadowManager::draw(
    float platformY, const PlayerSpriteRenderer& spriteRenderer) const {
    if (!shadow_.has_value()) {
        // 尚未生成时显示站立帧的半透明记录点模型。
        spriteRenderer.drawAt(
            recordedPlayerX_,
            recordedPlayerY_,
            Player::SPRITE_FRAME_WIDTH,
            Player::SPRITE_FRAME_HEIGHT,
            false,
            recordedFacingRight_,
            platformY,
            Color{120, 170, 255, 80});
        return;
    }

    const Rectangle hitbox{
        shadow_->getX(),
        platformY - shadow_->getHitboxHeight() - shadow_->getY(),
        shadow_->getHitboxWidth(),
        shadow_->getHitboxHeight()};
    const float timeRatio = 1.0F - elapsedLifetime_ / LIFETIME;
    const int barX = static_cast<int>(hitbox.x);
    const int timeBarY = static_cast<int>(hitbox.y - 11.0F);
    const int barWidth = static_cast<int>(shadow_->getHitboxWidth());

    // 影子复用玩家当前姿势；随后叠加原有红色碰撞箱和绿色存在时间条。
    spriteRenderer.draw(*shadow_, platformY, Color{150, 190, 255, 150});
    DrawRectangleRec(hitbox, Color{255, 100, 100, 60});
    DrawRectangleLinesEx(hitbox, 2.0F, RED);
    DrawRectangle(barX, timeBarY, barWidth, 4, DARKGRAY);
    DrawRectangle(barX, timeBarY,
                  static_cast<int>(barWidth * std::clamp(timeRatio, 0.0F, 1.0F)),
                  4, GREEN);
}

bool ShadowManager::hasShadow() const {
    return shadow_.has_value();
}

PlayerShadow* ShadowManager::getShadow() {
    return shadow_.has_value() ? &*shadow_ : nullptr;
}

const PlayerShadow* ShadowManager::getShadow() const {
    return shadow_.has_value() ? &*shadow_ : nullptr;
}
