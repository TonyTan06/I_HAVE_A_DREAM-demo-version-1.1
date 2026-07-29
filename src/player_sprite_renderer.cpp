#include "player_sprite_renderer.h"

#include "player.h"

PlayerSpriteRenderer::PlayerSpriteRenderer()
    : texture_{} {
}

PlayerSpriteRenderer::~PlayerSpriteRenderer() {
    unload();
}

bool PlayerSpriteRenderer::load(const char* filePath) {
    unload();
    if (filePath == nullptr || !FileExists(filePath)) return false;

    texture_ = LoadTexture(filePath);
    // 当前渲染规则要求图片能被均分成上下两帧。
    if (texture_.id == 0 || texture_.width <= 0 ||
        texture_.height <= 0 || texture_.height % FRAME_COUNT != 0) {
        unload();
        return false;
    }
    return true;
}

void PlayerSpriteRenderer::unload() {
    if (texture_.id == 0) return;

    UnloadTexture(texture_);
    texture_ = Texture2D{};
}

bool PlayerSpriteRenderer::isLoaded() const {
    return texture_.id != 0;
}

void PlayerSpriteRenderer::draw(
    const Player& player, float platformY, Color tint) const {
    drawAt(
        player.getX(),
        player.getY(),
        player.getHitboxWidth(),
        player.getHitboxHeight(),
        player.isMovingHorizontally(),
        player.isFacingRight(),
        platformY,
        tint);
}

void PlayerSpriteRenderer::drawAt(
    float x, float heightAboveGround,
    float hitboxWidth, float hitboxHeight,
    bool moving, bool facingRight,
    float platformY, Color tint) const {
    const Rectangle destination{
        x,
        platformY - hitboxHeight - heightAboveGround,
        hitboxWidth,
        hitboxHeight};

    if (!isLoaded()) {
        // 资源缺失时保留可见占位块，避免角色逻辑仍存在但画面完全透明。
        DrawRectangleRec(destination, tint);
        return;
    }

    const Rectangle source = makeSourceRectangle(
        static_cast<float>(texture_.width),
        static_cast<float>(texture_.height),
        moving,
        facingRight);
    DrawTexturePro(
        texture_, source, destination, Vector2{0.0F, 0.0F}, 0.0F, tint);
}

Rectangle PlayerSpriteRenderer::makeSourceRectangle(
    float textureWidth, float textureHeight,
    bool moving, bool facingRight) {
    const float frameHeight = textureHeight / static_cast<float>(FRAME_COUNT);
    const float frameY = moving ? frameHeight : 0.0F;
    const bool needsHorizontalFlip = facingRight != SOURCE_FACES_RIGHT;
    return Rectangle{
        0.0F,
        frameY,
        needsHorizontalFlip ? -textureWidth : textureWidth,
        frameHeight};
}
