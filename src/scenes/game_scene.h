#pragma once

#include "engine/scene.h"
#include "input/player_controller.h"
#include "rendering/character_renderer.h"
#include "rendering/hud_renderer.h"
#include "rendering/player_sprite_renderer.h"
#include "world/game_world.h"

#include <filesystem>

class GameScene final : public Scene {
public:
    GameScene();
    explicit GameScene(std::filesystem::path saveFilePath);

    bool load() override;
    void update(float deltaTime) override;
    void updateWithInput(const PlayerInputState& input, float deltaTime);
    void draw() const override;
    SceneSaveResult save() const override;

private:
    InputBindings inputBindings_;
    PlayerController playerController_;
    GameWorld gameWorld_;
    PlayerSpriteRenderer playerSpriteRenderer_;
    CharacterRenderer characterRenderer_;
    HudRenderer hudRenderer_;
    std::filesystem::path saveFilePath_;

    float damageTextElapsedTime_;
    float damageTextX_;
    float damageTextY_;
    float displayedDamage_;
    float attackEffectElapsedTime_;

    static constexpr float DAMAGE_TEXT_LIFETIME = 1.0F;
    static constexpr float ATTACK_EFFECT_LIFETIME = 0.15F;

    static void updateTimer(float& remainingTime, float deltaTime);
    void handleFrameResult(const GameWorldFrameResult& result);
    void showDamageText(float damage, float textX, float textY);
};
