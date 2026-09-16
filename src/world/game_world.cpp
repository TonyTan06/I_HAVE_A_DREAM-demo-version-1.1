#include "world/game_world.h"

#include <algorithm>
#include <utility>

namespace {

struct PlayerBlockAttempt {
    Player* player;

    bool operator()() const {
        return player != nullptr && player->blockNextAttack();
    }
};

} // namespace

GameWorld::GameWorld()
    : player_("PLAYER"),
      playerSpawnX_(player_.getX()),
      playerSpawnY_(player_.getY()),
      shadowManager_(player_),
      combatSystem_{},
      groundPlatform_{0.0F, 360.0F, 1200.0F, 40.0F} {
}

GameWorldFrameResult GameWorld::update(
    const PlayerInputState& input, float deltaTime) {
    GameWorldFrameResult frameResult;
    const PlayerActionFrameResult actions =
        playerActionSystem_.applyActions(player_, input, deltaTime);

    updatePlayerPhysics(deltaTime);
    spawnFriendlyProjectile(player_);
    if (PlayerShadow* shadow = shadowManager_.getShadow()) {
        spawnFriendlyProjectile(*shadow);
    }
    updateEnemyPhysicsAndCombat(deltaTime, frameResult);
    updateProjectiles(deltaTime, frameResult);
    updatePlayerMelee(actions.playerMeleeAttackRequested, frameResult);
    updateShadowLifecycleAndPhysics(deltaTime);
    respawnPlayerIfDefeated(frameResult);
    return frameResult;
}

Player& GameWorld::getPlayer() { return player_; }
const Player& GameWorld::getPlayer() const { return player_; }
PlayerShadow* GameWorld::getShadow() { return shadowManager_.getShadow(); }
const PlayerShadow* GameWorld::getShadow() const {
    return shadowManager_.getShadow();
}
const std::vector<std::unique_ptr<Enemy>>& GameWorld::getEnemies() const {
    return enemies_;
}
const Rectangle& GameWorld::getGroundPlatform() const {
    return groundPlatform_;
}
PlatformSystem& GameWorld::getPlatformSystem() { return platformSystem_; }
const PlatformSystem& GameWorld::getPlatformSystem() const {
    return platformSystem_;
}
ProjectileSystem& GameWorld::getProjectileSystem() {
    return projectileSystem_;
}
const ProjectileSystem& GameWorld::getProjectileSystem() const {
    return projectileSystem_;
}
CombatSystem& GameWorld::getCombatSystem() { return combatSystem_; }
const CombatSystem& GameWorld::getCombatSystem() const {
    return combatSystem_;
}
const ShadowManager& GameWorld::getShadowManager() const {
    return shadowManager_;
}

Rectangle GameWorld::makeCharacterHitbox(const Character& character) const {
    return Rectangle{
        character.getX(),
        groundPlatform_.y - character.getHitboxHeight() - character.getY(),
        character.getHitboxWidth(),
        character.getHitboxHeight()};
}

void GameWorld::updateCharacterPhysics(
    Character& character, float deltaTime) {
    platformSystem_.updateCharacterSupport(character, groundPlatform_.y);
    const float previousHeight = character.getY();
    character.update(deltaTime, WORLD_GRAVITY);
    platformSystem_.updateCharacterSupport(character, groundPlatform_.y);
    platformSystem_.resolveCharacterLanding(
        character, previousHeight, groundPlatform_.y);
    if (character.getY() <= 0.0F) character.land();
}

void GameWorld::updatePlayerPhysics(float deltaTime) {
    updateCharacterPhysics(player_, deltaTime);
    const float maxPlayerX = groundPlatform_.x + groundPlatform_.width -
        player_.getHitboxWidth();
    player_.setPosition(
        std::clamp(player_.getX(), groundPlatform_.x, maxPlayerX),
        player_.getY());
}

void GameWorld::spawnFriendlyProjectile(Player& attacker) {
    if (!attacker.consumeRangedAttackRequest()) return;

    const float direction = attacker.isFacingRight() ? 1.0F : -1.0F;
    const float projectileX = attacker.isFacingRight()
        ? attacker.getX() + attacker.getHitboxWidth() + PROJECTILE_RADIUS
        : attacker.getX() - PROJECTILE_RADIUS;
    projectileSystem_.spawn(ProjectileSystem::SpawnInfo{
        projectileX,
        groundPlatform_.y - attacker.getHitboxHeight() / 2.0F -
            attacker.getY(),
        direction,
        PROJECTILE_SPEED,
        std::min(PROJECTILE_MAX_DISTANCE, attacker.getRangedAttackRange()),
        PROJECTILE_RADIUS,
        1.0F, // TODO: remove the legacy SpawnInfo.damage field with save/UI migration.
        Faction::Friendly,
        ORANGE,
        0});
}

void GameWorld::spawnEnemyProjectile(Enemy& attacker) {
    const float direction = attacker.isFacingRight() ? 1.0F : -1.0F;
    const float projectileX = attacker.isFacingRight()
        ? attacker.getX() + attacker.getHitboxWidth() + PROJECTILE_RADIUS
        : attacker.getX() - PROJECTILE_RADIUS;
    projectileSystem_.spawn(ProjectileSystem::SpawnInfo{
        projectileX,
        groundPlatform_.y - attacker.getHitboxHeight() / 2.0F -
            attacker.getY(),
        direction,
        PROJECTILE_SPEED,
        std::min(PROJECTILE_MAX_DISTANCE, attacker.getRangedAttackRange()),
        PROJECTILE_RADIUS,
        1.0F,
        Faction::Enemy,
        Color{255, 45, 20, 255},
        0});
}

void GameWorld::updateEnemyPhysicsAndCombat(
    float deltaTime, GameWorldFrameResult& frameResult) {
    for (const std::unique_ptr<Enemy>& enemy : enemies_) {
        if (!enemy) continue;
        updateCharacterPhysics(*enemy, deltaTime);
        const CombatSystem::AttackResult meleeResult =
            combatSystem_.enemyMeleeAttack(*enemy, player_, groundPlatform_.y);
        if (meleeResult.hit) {
            recordDamage(
                frameResult, meleeResult.damage,
                meleeResult.targetHitbox.x +
                    meleeResult.targetHitbox.width / 2.0F,
                meleeResult.targetHitbox.y - 20.0F);
        }
        const CombatSystem::RangedAttackResult rangedResult =
            combatSystem_.tryEnemyRangedAttack(*enemy, player_);
        if (rangedResult.projectileRequested) {
            spawnEnemyProjectile(*enemy);
        }
    }
}

void GameWorld::updateProjectiles(
    float deltaTime, GameWorldFrameResult& frameResult) {
    std::vector<ProjectileSystem::Target> targets;
    targets.push_back(ProjectileSystem::Target{
        &player_,
        makeCharacterHitbox(player_),
        player_.isDodging(),
        player_.isDefending(),
        combatSystem_.makePlayerDefenseHitbox(player_, groundPlatform_.y),
        PlayerBlockAttempt{&player_}});
    for (const std::unique_ptr<Enemy>& enemy : enemies_) {
        if (!enemy) continue;
        targets.push_back(ProjectileSystem::Target{
            enemy.get(), makeCharacterHitbox(*enemy),
            false, false, Rectangle{}, {}});
    }

    const std::vector<ProjectileSystem::Impact> impacts =
        projectileSystem_.update(deltaTime, targets);
    for (const ProjectileSystem::Impact& impact : impacts) {
        if (!impact.blocked) {
            recordDamage(
                frameResult, impact.damage, impact.textX, impact.textY);
        }
    }
}

void GameWorld::updatePlayerMelee(
    bool requested, GameWorldFrameResult& frameResult) {
    if (!requested) return;

    std::vector<Enemy*> targets;
    for (const std::unique_ptr<Enemy>& enemy : enemies_) {
        if (enemy) targets.push_back(enemy.get());
    }
    const CombatSystem::AttackResult result =
        combatSystem_.playerMeleeAttack(
            player_, targets, groundPlatform_.y);
    frameResult.playerMeleeAttackPerformed = result.attackPerformed;
    if (result.hit) {
        recordDamage(
            frameResult, result.damage,
            result.targetHitbox.x + result.targetHitbox.width / 2.0F,
            result.targetHitbox.y - 20.0F);
    }
}

void GameWorld::respawnPlayerIfDefeated(
    GameWorldFrameResult& frameResult) {
    if (player_.isAlive()) return;
    frameResult.events.push_back(GameEvent::PlayerDied);
    player_.revive();
    player_.setPosition(playerSpawnX_, playerSpawnY_);
    player_.land();
    projectileSystem_.clear();
    shadowManager_.reset(player_);
    frameResult.events.push_back(GameEvent::PlayerRespawned);
}

void GameWorld::updateShadowLifecycleAndPhysics(float deltaTime) {
    PlayerShadow* shadowBeforeUpdate = shadowManager_.getShadow();
    const bool hadShadowBeforeUpdate = shadowBeforeUpdate != nullptr;
    const float previousShadowHeight = hadShadowBeforeUpdate
        ? shadowBeforeUpdate->getY()
        : 0.0F;
    if (shadowBeforeUpdate != nullptr) {
        platformSystem_.updateCharacterSupport(
            *shadowBeforeUpdate, groundPlatform_.y);
    }

    shadowManager_.update(player_, deltaTime, WORLD_GRAVITY);
    PlayerShadow* shadowAfterUpdate = shadowManager_.getShadow();
    if (shadowAfterUpdate == nullptr) return;

    platformSystem_.updateCharacterSupport(
        *shadowAfterUpdate, groundPlatform_.y);
    const float heightBeforePhysics = hadShadowBeforeUpdate
        ? previousShadowHeight
        : shadowAfterUpdate->getY();
    platformSystem_.resolveCharacterLanding(
        *shadowAfterUpdate, heightBeforePhysics, groundPlatform_.y);
    const float maxShadowX = groundPlatform_.x + groundPlatform_.width -
        shadowAfterUpdate->getHitboxWidth();
    shadowAfterUpdate->setPosition(
        std::clamp(shadowAfterUpdate->getX(), groundPlatform_.x, maxShadowX),
        shadowAfterUpdate->getY());
}

void GameWorld::recordDamage(
    GameWorldFrameResult& frameResult,
    float damage, float x, float y) {
    frameResult.damage = GameWorldFrameResult::DamageEvent{
        true, damage, x, y};
}
