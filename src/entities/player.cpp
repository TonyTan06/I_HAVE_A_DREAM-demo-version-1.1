#include "entities/player.h"

#include <algorithm>
#include <utility>
#include <iostream>
#include <cmath>

Player::Player(std::string name)
    : Character(std::move(name)),

      rangedAttackRequested_(false),
      isDefending_(false),
      isDodging_(false),
      dodgeRight_(true),
      isTakingTrueDamage_(false),

      defenseCooldown_(0.0F),
      dodgeElapsedTime_(0.0F),
      dodgeCooldown_(0.0F),
      lastDodgeDistance_(0.0F),
      defenseRange_(16.0F)
      {
        x_ = 140.0F;
        moveSpeed_ = 240.0F;
        meleeAttackInterval_ = 1.0F;
        rangedAttackInterval_ = 1.0F;
        meleeAttackCooldownRemaining_ = 0.0F;
        rangedAttackCooldownRemaining_ = 0.0F;
        hitboxWidth_ = SPRITE_FRAME_WIDTH;
        hitboxHeight_ = SPRITE_FRAME_HEIGHT;
        jumpInitialVelocity_ = 360.0F;
        meleeAttackRange_ = 48.0F;
        rangedAttackRange_ = 240.0F;

        health_ = 1;
        maxJumpCount_ = 2;

        setFaction(Faction::Friendly);
        setGravityScale(1.0F);
        // 当前测试素材的每一帧是 96×128，显示大小和碰撞箱保持一致。
        setHitboxSize(SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT);
      }
// 创建了主角，并且可以二连跳

void Player::update(float deltaTime, float worldGravity) {
    // 每帧重新记录，供 ShadowManager 区分普通移动与闪避移动。
    lastDodgeDistance_ = 0.0F;
    // 先更新角色共有的重力、跳跃等物理状态。
    Character::update(deltaTime, worldGravity);

    // 玩家自身的防御与闪避冷却按帧递减；攻击冷却由 Character 统一更新。
    defenseCooldown_ = std::max(0.0F, defenseCooldown_ - deltaTime);
    if (defenseCooldown_ > 0.0F) {
        isDefending_ = false;
    }
    dodgeCooldown_ = std::max(0.0F, dodgeCooldown_ - deltaTime);

    if (isDodging_) {
        // movementTime 防止低帧率时让最后一帧的移动超过 0.2 秒。
        const float movementTime = std::min(deltaTime, DODGE_DURATION - dodgeElapsedTime_);
        // 闪避总距离为“当前移速 × 1 秒”，因此必须在 DODGE_DURATION 内以该速度完成。
        const float dodgeSpeed = moveSpeed_ / DODGE_DURATION;
        lastDodgeDistance_ = dodgeSpeed * movementTime;
        x_ += (dodgeRight_ ? 1.0F : -1.0F) * lastDodgeDistance_;
        dodgeElapsedTime_ += movementTime;
        if (dodgeElapsedTime_ >= DODGE_DURATION) {
            isDodging_ = false;
        }
    }

    if (isGrounded_) jumpCount_ = 0;
} //主角的状态更新，主角独有的加在这里面

void Player::applyMeleeHit(Character& target) const {
    if (isTakingTrueDamage_) {
        target.takeTrueDamage();
    } else {
        target.takeDamage();
    }
}

bool Player::rangedAttack() {
    if (!tryBeginRangedAttack()) return false;

    rangedAttackRequested_ = true;
    return true;
}

void Player::setDefending(bool shouldDefend) {
    isDefending_ = shouldDefend && defenseCooldown_ <= 0.0F;
    if (isDefending_) {
        rangedAttackRequested_ = false;
    }
}

bool Player::isDefending() const {
    return isDefending_;
}

bool Player::isDodging() const {
    return isDodging_;
}

bool Player::isDodgeCoolingDown() const {
    return dodgeCooldown_ > 0.0F;
}

bool Player::consumeRangedAttackRequest() {
    const bool wasRequested = rangedAttackRequested_;
    rangedAttackRequested_ = false;
    return wasRequested;
}

bool Player::blockNextAttack() {
    if (!isDefending_) return false;

    isDefending_ = false;
    defenseCooldown_ = DEFENSE_COOLDOWN;
    return true;
}

bool Player::startDodge(bool dodgeRight) {
    // 闪避本体和闪避冷却期间均不能再次触发。
    if (isDodging_ || dodgeCooldown_ > 0.0F) return false;

    isDodging_ = true;
    isMovingHorizontally_ = true;
    dodgeRight_ = dodgeRight;
    dodgeElapsedTime_ = 0.0F;
    dodgeCooldown_ = DODGE_COOLDOWN;
    return true;
}

bool Player::tryBeginMeleeAttack() {
    if (isDefending_) return false;

    return Character::tryBeginMeleeAttack();
}

bool Player::tryBeginRangedAttack() {
    if (isDefending_) return false;

    return Character::tryBeginRangedAttack();
}

float Player::getDodgeCooldownProgress() const {
    // 绘制时用“已冷却比例”，所以条会从空逐渐填满。
    return 1.0F - dodgeCooldown_ / DODGE_COOLDOWN;
}

float Player::getLastDodgeDistance() const {
    return lastDodgeDistance_;
}

float Player::getDefenseRange() const {
    return defenseRange_;
}

void Player::copyMovementStateFrom(const Player& player) {
    jumpCount_ = player.jumpCount_;
    maxJumpCount_ = player.maxJumpCount_;
    isFacingRight_ = player.isFacingRight_;
    isMovingHorizontally_ = player.isMovingHorizontally_;
}

void Player::copyAttributesFrom(const Player& player) {
    moveSpeed_ = player.getMoveSpeed();
    meleeAttackInterval_ = player.getMeleeAttackInterval();
    rangedAttackInterval_ = player.getRangedAttackInterval();
    hitboxWidth_ = player.getHitboxWidth();
    hitboxHeight_ = player.getHitboxHeight();
    jumpInitialVelocity_ = player.getJumpInitialVelocity();
    gravityScale_ = player.getGravityScale();
}
