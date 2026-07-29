#include "character.h"
#include <utility>
#include <algorithm>

Character::Character(std::string name)
    : name_(std::move(name)),
      x_(0.0F),
      y_(0.0F),
      moveSpeed_(0.0F),
      physicalDefense_(0.0F),
      magicalDefense_(0.0F),
      health_(1.0F),
      maxHealth_(1.0F),
      attackDamage_(0.0F),
      attacksPerSecond_(0.0F),
      shield_(0.0F),
      hitboxWidth_(32.0F),
      hitboxHeight_(48.0F),
      jumpInitialVelocity_(0.0F),
      verticalVelocity_(0.0F),
      gravityScale_(1.0F),
      isGrounded_(true),
      faction_(Faction::Neutral)
{
} //输入角色名字就创建了，里面包含基础属性，具体数值不在这里调整，这只是通用配置。

void Character::moveRight(float deltaTime) {
    x_ += moveSpeed_ * deltaTime;
} //右移

void Character::moveLeft(float deltaTime) {
    x_ -= moveSpeed_ * deltaTime;
} //左移
//deltaTime 是上一帧到当前帧经过了多少秒。

void Character::jump() {
    if (!isGrounded_) return;

    verticalVelocity_ = jumpInitialVelocity_;
    isGrounded_ = false;

} //跳跃

void Character::takeDamage(float damage) {
    if (damage <= 0.0F) return;

    health_ = std::max(0.0F, health_ - damage);
} //受击伤害

void Character::heal(float value) {
    if (value <= 0.0F) return;

    health_ = std::min(maxHealth_, health_ + value);
} //回血量
    
void Character::attack() {
    
}

void Character::rangedAttack() {
    // 基类不定义远程攻击表现，保留给 Player、Enemy 等派生类覆写。
}

bool Character::isAlive() const {
    return health_ > 0.0F;
} //检测存活

bool Character::isGrounded() const {
    return isGrounded_;
}

float Character::getHealth() const {
    return health_;
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
}

void Character::beginFalling() {
    if (!isGrounded_) return;

    verticalVelocity_ = 0.0F;
    isGrounded_ = false;
}

void Character::swapPositionAndPhysics(Character& other) {
    std::swap(x_, other.x_);
    std::swap(y_, other.y_);
    std::swap(verticalVelocity_, other.verticalVelocity_);
    std::swap(isGrounded_, other.isGrounded_);
}

float Character::getMoveSpeed() const {
    return moveSpeed_;
}

float Character::getPhysicalDefense() const {
    return physicalDefense_;
}

float Character::getMagicalDefense() const {
    return magicalDefense_;
}

float Character::getMaxHealth() const {
    return maxHealth_;
}

float Character::getAttackDamage() const {
    return attackDamage_;
}

float Character::getAttacksPerSecond() const {
    return attacksPerSecond_;
}

float Character::getShield() const {
    return shield_;
}

float Character::getJumpInitialVelocity() const {
    return jumpInitialVelocity_;
}

Faction Character::getFaction() const {
    return faction_;
}

void Character::setFaction(Faction faction) {
    faction_ = faction;
}

float Character::getGravityScale() const {
    return gravityScale_;
} //获取角色自身重力倍率

void Character::setGravityScale(float gravityScale) {
    if (gravityScale < 0.0F) return;

    gravityScale_ = gravityScale;
} //设置角色自身重力倍率

void Character::setHitboxSize(float width, float height) {
    if (width <= 0.0F || height <= 0.0F) return;

    hitboxWidth_ = width;
    hitboxHeight_ = height;
}

void Character::update(float deltaTime) {
    if (!isAlive()) return;

    const float actualGravity = BASE_GRAVITY * gravityScale_;

    if (!isGrounded_) {
        verticalVelocity_ -= actualGravity * deltaTime;
        y_ += verticalVelocity_ * deltaTime;
    } //跳跃更新函数，当不在地面上时，竖直方向上的速度=加速度对时间的积分，竖直高度=速度对时间的积分

    
} //每一帧更新角色函数, 很重要！！！！所有角色的通用逻辑都在这里面
