#pragma once

#include "raylib.h"

class Player;

// PlayerSpriteRenderer 只负责玩家双帧素材的加载、帧裁切、朝向翻转和绘制。
// 玩家移动、技能和碰撞逻辑仍保存在 Player 与各系统中。
class PlayerSpriteRenderer {
public:
    PlayerSpriteRenderer(); // 创建尚未加载纹理的安全空渲染器
    ~PlayerSpriteRenderer(); // 在 raylib 窗口关闭前释放显存中的玩家纹理

    PlayerSpriteRenderer(const PlayerSpriteRenderer&) = delete;
    PlayerSpriteRenderer& operator=(const PlayerSpriteRenderer&) = delete;

    bool load(const char* filePath); // 加载竖向双帧 PNG；成功时返回 true
    void unload(); // 主动释放纹理；未加载时调用不会产生影响
    bool isLoaded() const; // 当前是否持有可供绘制的 raylib 纹理

    // 绘制玩家或 PlayerShadow；碰撞箱大小即目标绘制大小。
    void draw(const Player& player, float platformY, Color tint) const;
    // 绘制尚未生成的影子记录点，位置与姿势由 ShadowManager 保存。
    void drawAt(float x, float heightAboveGround,
                float hitboxWidth, float hitboxHeight,
                bool moving, bool facingRight,
                float platformY, Color tint) const;

    // 纯计算接口供单元测试验证：第一帧站立、第二帧行走，并按朝向水平翻转。
    static Rectangle makeSourceRectangle(
        float textureWidth, float textureHeight,
        bool moving, bool facingRight);

private:
    Texture2D texture_; // raylib 上传到显存的完整 96×256 双帧纹理

    static constexpr int FRAME_COUNT = 2; // 素材沿竖直方向依次排列两帧
    static constexpr bool SOURCE_FACES_RIGHT = true; // 原始素材人物面向右侧
};
