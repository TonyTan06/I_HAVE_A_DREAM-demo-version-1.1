#include "entities/player_shadow.h"

#include "entities/player.h"

#include <algorithm>

PlayerShadow::PlayerShadow(const Player& player, float x, float y)
    : Player("PLAYER_SHADOW"),
      activeSkill_(ShadowSkill::SynchronizePlayerActions),
      positionSwapCooldown_(0.0F) {
    // 位置来自上一次记录点，而非本次生成时玩家所在的位置。
    setPosition(x, y);
    copyMovementStateFrom(player);
    // 影子的属性在生成瞬间固定，不会随玩家之后的属性变化而改变。
    moveSpeed_ = player.getMoveSpeed();
    physicalDefense_ = player.getPhysicalDefense();
    magicalDefense_ = player.getMagicalDefense();
    attackDamage_ = player.getAttackDamage();
    attacksPerSecond_ = player.getAttacksPerSecond();
    shield_ = player.getShield();
    jumpInitialVelocity_ = player.getJumpInitialVelocity();
    gravityScale_ = player.getGravityScale();
    // 空中生成的影子进入重力更新；地面生成的影子直接保持落地。
    isGrounded_ = y <= 0.0F;
    applyActiveSkillState();
}

void PlayerShadow::takeDamage(float damage) {
    // 明确忽略参数：影子没有血量，不接受任何来源的伤害。
    (void)damage;
}

void PlayerShadow::update(float deltaTime) {
    // 先更新继承自 Player 的重力、动作和普通冷却，再更新影子技能冷却。
    Player::update(deltaTime);
    positionSwapCooldown_ =
        std::max(0.0F, positionSwapCooldown_ - std::max(0.0F, deltaTime));
}

void PlayerShadow::setActiveSkill(ShadowSkill skill) {
    // 技能状态属于当前影子实体，ShadowManager 不参与技能选择。
    activeSkill_ = skill;
    applyActiveSkillState();
}

ShadowSkill PlayerShadow::getActiveSkill() const {
    return activeSkill_;
}

bool PlayerShadow::applyInput(
    const PlayerInputState& input,
    HorizontalInputDirection horizontalDirection,
    float deltaTime) {
    if (activeSkill_ == ShadowSkill::SynchronizePlayerActions) {
        return Player::applyInput(input, horizontalDirection, deltaTime);
    }
    return false;
}

bool PlayerShadow::tryUsePositionSwap(Player& player) {
    // 冷却期间按数字键 2 不产生交换，也不会重置剩余冷却。
    if (positionSwapCooldown_ > 0.0F) return false;

    // 交换包含 x/y、竖直速度与 isGrounded，确保空中状态跟随位置一起交换。
    setActiveSkill(ShadowSkill::SwapPosition);
    swapPositionAndPhysics(player);
    positionSwapCooldown_ = POSITION_SWAP_COOLDOWN;
    // 技能 2 是瞬时技能，交换完成后继续执行技能 1 的同步行为。
    setActiveSkill(ShadowSkill::SynchronizePlayerActions);
    return true;
}

bool PlayerShadow::isPositionSwapCoolingDown() const {
    return positionSwapCooldown_ > 0.0F;
}

float PlayerShadow::getPositionSwapCooldownProgress() const {
    // 刚释放时为 0，十秒内逐渐增加到 1，供 UI 从下向上填充。
    return 1.0F - positionSwapCooldown_ / POSITION_SWAP_COOLDOWN;
}

void PlayerShadow::applyActiveSkillState() {
    if (activeSkill_ == ShadowSkill::SynchronizePlayerActions) {
        setFaction(Faction::Friendly);
    }
}
