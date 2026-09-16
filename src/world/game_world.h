#pragma once

#include "engine/game_event.h"
#include "entities/enemy.h"
#include "entities/player.h"
#include "input/player_input_state.h"
#include "raylib.h"
#include "systems/combat_system.h"
#include "systems/platform_system.h"
#include "systems/player_action_system.h"
#include "systems/projectile_system.h"
#include "systems/shadow_manager.h"

#include <memory>
#include <vector>

// 返回当前帧的局部反馈和真正发生的全局 gameplay 事件。
// events 仅向上层报告事件，不是 EventBus。
struct GameWorldFrameResult {
    struct DamageEvent {
        bool occurred = false;
        float damage = 0.0F;
        float x = 0.0F;
        float y = 0.0F;
    } damage;

    bool playerMeleeAttackPerformed = false;
    std::vector<GameEvent> events;
};

// 当前运行中的 gameplay 世界；输入读取、绘制和存档由 GameScene 负责。
class GameWorld {
public:
    GameWorld();

    GameWorldFrameResult update(
        const PlayerInputState& input, float deltaTime);

    Player& getPlayer();
    const Player& getPlayer() const;
    PlayerShadow* getShadow();
    const PlayerShadow* getShadow() const;
    const std::vector<std::unique_ptr<Enemy>>& getEnemies() const;
    const Rectangle& getGroundPlatform() const;
    PlatformSystem& getPlatformSystem();
    const PlatformSystem& getPlatformSystem() const;
    ProjectileSystem& getProjectileSystem();
    const ProjectileSystem& getProjectileSystem() const;
    CombatSystem& getCombatSystem();
    const CombatSystem& getCombatSystem() const;
    const ShadowManager& getShadowManager() const;

private:
    Player player_;
    float playerSpawnX_;
    float playerSpawnY_;
    ShadowManager shadowManager_;
    PlayerActionSystem playerActionSystem_;
    CombatSystem combatSystem_;
    ProjectileSystem projectileSystem_;
    PlatformSystem platformSystem_;
    Rectangle groundPlatform_;
    std::vector<std::unique_ptr<Enemy>> enemies_;

    static constexpr float WORLD_GRAVITY = 980.0F;
    static constexpr float PROJECTILE_RADIUS = 4.0F;
    static constexpr float PROJECTILE_SPEED = 300.0F;
    static constexpr float PROJECTILE_MAX_DISTANCE = 500.0F;

    void updateCharacterPhysics(Character& character, float deltaTime);
    void updatePlayerPhysics(float deltaTime);
    void updateEnemyPhysicsAndCombat(
        float deltaTime, GameWorldFrameResult& frameResult);
    void spawnFriendlyProjectile(Player& attacker);
    void spawnEnemyProjectile(Enemy& attacker);
    void updateProjectiles(
        float deltaTime, GameWorldFrameResult& frameResult);
    void updatePlayerMelee(
        bool requested, GameWorldFrameResult& frameResult);
    void updateShadowLifecycleAndPhysics(float deltaTime);
    void respawnPlayerIfDefeated(GameWorldFrameResult& frameResult);
    Rectangle makeCharacterHitbox(const Character& character) const;
    static void recordDamage(
        GameWorldFrameResult& frameResult,
        float damage, float x, float y);
};
