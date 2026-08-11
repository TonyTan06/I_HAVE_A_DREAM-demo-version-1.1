#include "entities/character.h"
#include <utility>
#include <algorithm>

Character::Character(std::string name)
    : name_(std::move(name)),

      isGrounded_(true),
      isFacingRight_(true),
      isMovingHorizontally_(false),

      x_(0.0F),
      y_(0.0F),
      moveSpeed_(0.0F),
      meleeAttackInterval_(1.0F),
      rangedAttackInterval_(1.0F),
      meleeAttackCooldownRemaining_(0.0F),
      rangedAttackCooldownRemaining_(0.0F),
      hitboxWidth_(0.0F),
      hitboxHeight_(0.0F),
      jumpInitialVelocity_(0.0F),
      verticalVelocity_(0.0F),
      gravityScale_(1.0F),
      meleeAttackRange_(0.0F),
      rangedAttackRange_(0.0F),

      health_(1),
      shield_(0),
      jumpCount_(0),
      maxJumpCount_(1),

      faction_(Faction::Neutral)
{
} //输入角色名字就创建了，里面包含基础属性，具体数值不在这里调整，这只是通用配置。

void Character::moveRight(float deltaTime) {
    isFacingRight_ = true;
    isMovingHorizontally_ = true;
    x_ += moveSpeed_ * deltaTime;
} //右移

void Character::moveLeft(float deltaTime) {
    isFacingRight_ = false;
    isMovingHorizontally_ = true;
    x_ -= moveSpeed_ * deltaTime;
} //左移
//deltaTime 是上一帧到当前帧经过了多少秒。

void Character::jump() {
    if (jumpCount_ >= maxJumpCount_) return;

    verticalVelocity_ = jumpInitialVelocity_;
    isGrounded_ = false;
    ++ jumpCount_;

} //跳跃

void Character::takeDamage() {
    if (health_ > 0 && Character::getShield() > 0) shield_ -= 1;
    else if (health_ > 0) health_ -= 1;
} //受击伤害

void Character::takeTrueDamage() {
   if (health_ > 0) health_ -= 1;
}

void Character::update(float deltaTime, float worldGravity) {
    if (!isAlive()) return;

    const float cooldownDeltaTime = std::max(0.0F, deltaTime);
    
    meleeAttackCooldownRemaining_ = std::max(0.0F, meleeAttackCooldownRemaining_ - cooldownDeltaTime);
    rangedAttackCooldownRemaining_ = std::max(0.0F, rangedAttackCooldownRemaining_ - cooldownDeltaTime);

    const float actualGravity = worldGravity * gravityScale_;
    if (!isGrounded_) {
        verticalVelocity_ -= actualGravity * deltaTime;
        y_ += verticalVelocity_ * deltaTime;
    } //跳跃更新函数，当不在地面上时，竖直方向上的速度=加速度对时间的积分，竖直高度=速度对时间的积分
    if (isGrounded()) {
        jumpCount_ = 0;
    }
} //每一帧更新角色函数, 很重要！！！！所有角色的通用逻辑都在这里面

void Character::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

void Character::land() {
    landAtHeight(0.0F);
}

void Character::landAtHeight(float height) {
    y_ = std::max(0.0F, height);
    verticalVelocity_ = 0.0F;
    isGrounded_ = true;
    jumpCount_ = 0;
}

void Character::beginFalling() {
    if (!isGrounded_) return;

    verticalVelocity_ = 0.0F;
    isGrounded_ = false;
}

void Character::setFaction(Faction faction) {
    faction_ = faction;
}

void Character::setGravityScale(float gravityScale) {
    if (gravityScale < 0.0F) return;

    gravityScale_ = gravityScale;
} //设置角色自身重力倍率

void Character::stopMovingHorizontally() {
    isMovingHorizontally_ = false;
}

bool Character::tryBeginMeleeAttack() {
    if (!isAlive() || !isMeleeAttackReady()) return false;

    meleeAttackCooldownRemaining_ = std::max(0.0F, meleeAttackInterval_);
    return true;
}

bool Character::tryBeginRangedAttack() {
    if (!isAlive() || !isRangedAttackReady()) return false;

    rangedAttackCooldownRemaining_ = std::max(0.0F, rangedAttackInterval_);
    return true;
}

bool Character::isAlive() const {
    return health_ > 0;
} //检测存活

bool Character::isGrounded() const {
    return isGrounded_;
}

bool Character::isFacingRight() const {
    return isFacingRight_;
}

bool Character::isMovingHorizontally() const {
    return isMovingHorizontally_;
}

bool Character::isMeleeAttackReady() const {
    return meleeAttackCooldownRemaining_ <= 0.0F;
}

bool Character::isRangedAttackReady() const {
    return rangedAttackCooldownRemaining_ <= 0.0F;
}

float Character::getX() const {
    return x_;
}

float Character::getY() const {
    return y_;
}

float Character::getHitboxWidth() const {
    return hitboxWidth_;
}

float Character::getHitboxHeight() const {
    return hitboxHeight_;
}

float Character::getMoveSpeed() const {
    return moveSpeed_;
}

float Character::getMeleeAttackInterval() const {
    return meleeAttackInterval_;
}

float Character::getRangedAttackInterval() const {
    return rangedAttackInterval_;
}

float Character::getJumpInitialVelocity() const {
    return jumpInitialVelocity_;
}

float Character::getGravityScale() const {
    return gravityScale_;
} //获取角色自身重力倍率

float Character::getMeleeAttackRange() const {
    return meleeAttackRange_;
} // 近战攻击的有效范围，单位：像素

float Character::getRangedAttackRange() const {
    return rangedAttackRange_;
} // 远程攻击的有效范围，单位：像素


int Character::getHealth() const {
    return health_;
}

int Character::getShield() const {
    return shield_;
}

int Character::getJumpCount() const {
    return jumpCount_;
}

int Character::getMaxJumpCount() const {
    return maxJumpCount_;
}

Faction Character::getFaction() const {
    return faction_;
}

void Character::setHitboxSize(float width, float height) {
    if (width <= 0.0F || height <= 0.0F) return;

    hitboxWidth_ = width;
    hitboxHeight_ = height;
}
