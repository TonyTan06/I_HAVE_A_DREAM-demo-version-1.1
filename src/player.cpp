#include "player.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include <cmath>

Player::Player(std::string name)
    : Character(std::move(name)),
      jumpCount_(0),
      maxJumpCount_(2),
      experience_(0),
      level_(1),
      gold_(0),
      isFacingRight_(true), // 初始朝右，使眼睛、攻击和远程子弹都默认向右
      isMovingHorizontally_(false),
      rangedAttackCooldown_(0.0F),
      rangedAttackRequested_(false),
      isDefending_(false),
      defenseCooldown_(0.0F),
      isDodging_(false),
      dodgeRight_(true),
      dodgeElapsedTime_(0.0F),
      dodgeCooldown_(0.0F),
      lastDodgeDistance_(0.0F) {
        setGravityScale(1.0F);
        setFaction(Faction::Friendly);
        x_ = 140.0F;
        moveSpeed_ = 240.0F;
        health_ = 10.0F;
        maxHealth_ = 10.0F;
        attackDamage_ = 5.0F;
        jumpInitialVelocity_ = 360.0F;
        // 当前测试素材的每一帧是 96×128，显示大小和碰撞箱保持一致。
        setHitboxSize(SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT);
      }
// 创建了主角，并且可以二连跳

void Player::moveRight(float deltaTime) {
    isFacingRight_ = true;
    Character::moveRight(deltaTime);
} //主角右移

void Player::moveLeft(float deltaTime) {
    isFacingRight_ = false;
    Character::moveLeft(deltaTime);
} //主角左移

void Player::jump() {
    if (jumpCount_ >= maxJumpCount_) return;

    verticalVelocity_ = jumpInitialVelocity_;
    isGrounded_ = false;
    ++jumpCount_;
} //角色跳跃

void Player::update(float deltaTime) {
    // 每帧重新记录，供 ShadowManager 区分普通移动与闪避移动。
    lastDodgeDistance_ = 0.0F;
    // 先更新角色共有的重力、跳跃等物理状态。
    Character::update(deltaTime);

    // 两个普通冷却按每帧经过的时间递减，且不会低于零。
    rangedAttackCooldown_ = std::max(0.0F, rangedAttackCooldown_ - deltaTime);
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

void Player::addGold(int value) {
    if (value <= 0) return;

    gold_ += value;
}

void Player::addExperience(int value) {
    if (value <= 0) return;

    experience_ += value;

    while (experience_ >= getExperienceThreshold()) {
        levelUp();
    }
}

void Player::levelUp() {
    experience_ -= getExperienceThreshold();
    ++level_;
    maxHealth_ += 5.0F;
    health_ = maxHealth_;
    attackDamage_ += 1.0F;
    attacksPerSecond_ += 0.1F;
    /*
    physicalDefense_ += ;
    magicalDefense_ += ;
    */

    std::cout << "LEVEL UP!" << std::endl;
}

void Player::attack() {
    std::cout << "Attack!!!" << std::endl;
}

void Player::attack(Character& target) {
    target.takeDamage(attackDamage_);
}

void Player::rangedAttack() {
    if (isDefending_) return;
    if (rangedAttackCooldown_ > 0.0F) return;

    rangedAttackCooldown_ = RANGED_ATTACK_INTERVAL;
    rangedAttackRequested_ = true;
}

bool Player::isFacingRight() const {
    return isFacingRight_;
}

bool Player::isMovingHorizontally() const {
    return isMovingHorizontally_;
}

bool Player::consumeRangedAttackRequest() {
    const bool wasRequested = rangedAttackRequested_;
    rangedAttackRequested_ = false;
    return wasRequested;
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
    dodgeRight_ = dodgeRight;
    dodgeElapsedTime_ = 0.0F;
    dodgeCooldown_ = DODGE_COOLDOWN;
    return true;
}

bool Player::isDodging() const {
    return isDodging_;
}

bool Player::isDodgeCoolingDown() const {
    return dodgeCooldown_ > 0.0F;
}

float Player::getDodgeCooldownProgress() const {
    // 绘制时用“已冷却比例”，所以条会从空逐渐填满。
    return 1.0F - dodgeCooldown_ / DODGE_COOLDOWN;
}

float Player::getLastDodgeDistance() const {
    return lastDodgeDistance_;
}

bool Player::applyInput(const PlayerInputState& input,
                        HorizontalInputDirection horizontalDirection,
                        float deltaTime) {
    if (!isDodging()) {
        if (horizontalDirection == HorizontalInputDirection::Left) {
            moveLeft(deltaTime);
        } else if (horizontalDirection == HorizontalInputDirection::Right) {
            moveRight(deltaTime);
        }
    }
    if (input.jumpPressed) {
        jump();
    }

    setDefending(input.defendHeld);
    const bool meleeAttackPressed = input.meleeAttackPressed && !isDefending();
    if (input.rangedAttackPressed && !isDefending()) {
        rangedAttack();
    }
    if (input.dodgePressed) {
        const bool dodgeRight = horizontalDirection == HorizontalInputDirection::Right ||
            (horizontalDirection == HorizontalInputDirection::None && isFacingRight());
        startDodge(dodgeRight);
    }
    // 两个方向键同时按下时方向为 None，因此显示站立帧并保留原朝向。
    // 闪避即使没有持续按方向键，也属于水平移动并显示行走姿势。
    isMovingHorizontally_ =
        horizontalDirection != HorizontalInputDirection::None || isDodging();
    return meleeAttackPressed;
}

void Player::copyMovementStateFrom(const Player& player) {
    jumpCount_ = player.jumpCount_;
    maxJumpCount_ = player.maxJumpCount_;
    isFacingRight_ = player.isFacingRight_;
    isMovingHorizontally_ = player.isMovingHorizontally_;
}

int Player::getExperienceThreshold() const{
    return static_cast<int>(10.0F * (level_ + level_ * std::log(level_)));
}

int Player::getJumpCount() const {
    return jumpCount_;
} //获取当前跳跃次数

int Player::getMaxJumpCount() const {
    return maxJumpCount_;
} //获取最大跳跃次数

int Player::getGold() const {
    return gold_;
} //获取当前金币数量

int Player::getExperience() const {
    return experience_;
} //显示当前经验值

int Player::getLevel() const {
    return level_;
} //显示当前等级
