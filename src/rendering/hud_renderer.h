#pragma once

class Player;

struct DamageTextView {
    bool visible;
    float damage;
    float x;
    float y;
};

// 绘制不参与游戏规则计算的屏幕信息和短暂伤害提示。
class HudRenderer {
public:
    void draw(const Player& player, const DamageTextView& damageText) const;
};
