#pragma once

#include "raylib.h"

#include <vector>

class Character;

// PlatformSystem 统一保存单向平台，并处理角色站立、离开和下落碰撞。
class PlatformSystem {
public:
    void addPlatform(const Rectangle& platform); // 添加一块使用屏幕坐标表示的平台
    void draw(Color color) const; // 使用指定颜色绘制系统内的全部平台

    // 角色站在高处却已离开平台水平范围时，将角色切换为下落状态。
    void updateCharacterSupport(Character& character, float groundY) const;

    // 角色本帧从平台顶面上方向下穿过时，将其落在最高的有效平台上。
    void resolveCharacterLanding(
        Character& character, float previousHeight, float groundY) const;

    const std::vector<Rectangle>& getPlatforms() const; // 返回平台列表供测试或后续查询

private:
    std::vector<Rectangle> platforms_; // 当前场景中的全部单向平台

    static bool overlapsHorizontally(
        const Character& character,
        const Rectangle& platform); // 使用角色自身碰撞箱宽度判断是否与平台重叠
};
