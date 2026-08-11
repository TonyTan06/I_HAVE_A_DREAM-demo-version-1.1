#include "entities/player_shadow.h"

#include "entities/player.h"

#include <algorithm>

PlayerShadow::PlayerShadow(const Player& player, float x, float y)
    : Player("PLAYER_SHADOW")
    {
    // 位置来自上一次记录点，而非本次生成时玩家所在的位置。
    setPosition(x, y);
    copyMovementStateFrom(player);
    // 影子只复制攻击间隔等稳定属性；剩余冷却属于当前实体并从可攻击状态开始。
    // 这些属性在生成瞬间固定，不会随玩家之后的属性变化而改变。
    copyAttributesFrom(player);
    // 空中生成的影子进入重力更新；地面生成的影子直接保持落地。
    isGrounded_ = y <= 0.0F;

    setFaction(Faction::Neutral);
}

void PlayerShadow::takeDamage() {
    return; // 影子没有血量概念，任何直接伤害调用都会被忽略。
}

void PlayerShadow::takeTrueDamage() {
    return; // 影子没有血量概念，任何直接伤害调用都会被忽略。
}

void PlayerShadow::update(float deltaTime, float worldGravity) {
    // 先更新继承自 Player 的重力、动作和普通冷却，再更新影子技能冷却。
    Player::update(deltaTime, worldGravity);
}