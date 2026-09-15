#include "scenes/game_scene.h"

#include "systems/save_game_store.h"

#include <algorithm>
#include <string>
#include <utility>

GameScene::GameScene()
    : GameScene(SaveGameStore::getDefaultSavePath()) {
}

GameScene::GameScene(std::filesystem::path saveFilePath)
    : inputBindings_{},
      playerController_(inputBindings_),
      gameWorld_{},
      saveFilePath_(std::move(saveFilePath)),
      damageTextElapsedTime_(0.0F),
      damageTextX_(0.0F),
      damageTextY_(0.0F),
      displayedDamage_(0.0F),
      attackEffectElapsedTime_(0.0F) {
}

bool GameScene::load() {
    if (playerSpriteRenderer_.load("assets/移动.png")) return true;

    const std::string executableRelativePath =
        std::string(GetApplicationDirectory()) + "assets/移动.png";
    return playerSpriteRenderer_.load(executableRelativePath.c_str());
}

void GameScene::update(float deltaTime) {
    const PlayerInputState input = playerController_.pollInput();
    updateWithInput(input, deltaTime);
}

void GameScene::updateWithInput(
    const PlayerInputState& input, float deltaTime) {
    updateTimer(damageTextElapsedTime_, deltaTime);
    updateTimer(attackEffectElapsedTime_, deltaTime);

    const GameWorldFrameResult frameResult =
        gameWorld_.update(input, deltaTime);
    if (frameResult.playerMeleeAttackPerformed) {
        attackEffectElapsedTime_ = ATTACK_EFFECT_LIFETIME;
    }
    if (frameResult.damage.occurred) {
        showDamageText(
            frameResult.damage.damage,
            frameResult.damage.x,
            frameResult.damage.y);
    }
}

void GameScene::draw() const {
    ClearBackground(Color{24, 28, 38, 255});

    const Rectangle& ground = gameWorld_.getGroundPlatform();
    DrawRectangleRec(ground, Color{126, 91, 67, 255});
    gameWorld_.getPlatformSystem().draw(Color{126, 91, 67, 255});
    gameWorld_.getShadowManager().draw(ground.y, playerSpriteRenderer_);

    for (const std::unique_ptr<Enemy>& enemy : gameWorld_.getEnemies()) {
        if (enemy) {
            characterRenderer_.drawEnemy(
                *enemy, ground.y, BLUE, false,
                enemy->getMeleeAttackRange());
        }
    }

    const Player& player = gameWorld_.getPlayer();
    characterRenderer_.drawPlayer(
        player, ground.y, playerSpriteRenderer_,
        attackEffectElapsedTime_ > 0.0F,
        player.getMeleeAttackRange(), player.getDefenseRange());
    if (const PlayerShadow* shadow = gameWorld_.getShadow()) {
        characterRenderer_.drawShadow(
            *shadow, ground.y,
            false,
            shadow->getMeleeAttackRange(), shadow->getDefenseRange());
    }

    gameWorld_.getProjectileSystem().draw();
    hudRenderer_.draw(DamageTextView{
        damageTextElapsedTime_ > 0.0F,
        displayedDamage_, damageTextX_, damageTextY_});
}

SceneSaveResult GameScene::save() const {
    const Player& player = gameWorld_.getPlayer();
    SaveGameSnapshot snapshot;
    snapshot.sceneId = "game_scene";
    snapshot.player.x = player.getX();
    snapshot.player.y = player.getY();
    snapshot.player.health = player.getHealth();
    snapshot.player.facingRight = player.isFacingRight();

    return SaveGameStore::save(snapshot, saveFilePath_)
        ? SceneSaveResult::Success
        : SceneSaveResult::Failed;
}

void GameScene::updateTimer(float& remainingTime, float deltaTime) {
    if (remainingTime <= 0.0F) return;
    remainingTime = std::max(0.0F, remainingTime - deltaTime);
}

void GameScene::showDamageText(
    float damage, float textX, float textY) {
    displayedDamage_ = damage;
    damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
    damageTextX_ = textX;
    damageTextY_ = textY;
}
