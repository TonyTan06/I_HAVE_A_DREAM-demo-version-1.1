#pragma once

#include "entities/player.h"

// PlayerShadow 继承 Player，因此能够复用移动、跳跃、防御、闪避和攻击请求。
// 它不处理生成与销毁；这些生命周期职责由 ShadowManager 负责。
class PlayerShadow : public Player {
public:
    // 在记录点创建影子，并快照玩家当下的战斗与跳跃属性。
    PlayerShadow(const Player& player, float x, float y);

    // 影子没有血量概念，任何直接伤害调用都会被忽略。
    void takeDamage() override;
    void takeTrueDamage() override;
    void update(float deltaTime, float worldGravity) override; // 更新影子自身动作状态与技能 2 冷却

private:

};
