#include "scenes/game_scene.h"

#include <algorithm>
#include <string>
#include <vector>

GameScene::GameScene()
    : player_("PLAYER"),
      meleeEnemy_(player_),
      rangedEnemy_(player_),
      playerSpawnX_(player_.getX()),
      playerSpawnY_(player_.getY()),
      shadowManager_(player_),
      combatSystem_(16.0F, 8.0F),
      platform_{0.0F, 360.0F, 1200.0F, 40.0F},
      damageTextElapsedTime_(0.0F),
      damageTextX_(0.0F),
      damageTextY_(0.0F),
      displayedDamage_(0.0F),
      attackEffectElapsedTime_(0.0F),
      shadowAttackEffectElapsedTime_(0.0F),
      meleeAttackEffectElapsedTime_(0.0F),
      meleeEnemyExperienceAwarded_(false),
      rangedEnemyExperienceAwarded_(false) {
    // 平台加长时不移动已有敌人；近战兵仍在远程兵左方 100px。
    rangedEnemy_.setPosition(RANGED_ENEMY_SPAWN_X, 0.0F);
    meleeEnemy_.setPosition(rangedEnemy_.getX() - ENEMY_SPACING, 0.0F);

    // 两块小平台分别以敌军身体中心为水平中心，顶面离主地面 60px。
    const float upperPlatformY = platform_.y - UPPER_PLATFORM_HEIGHT;
    const float halfPlatformWidth = UPPER_PLATFORM_WIDTH / 2.0F;
    platformSystem_.addPlatform(Rectangle{
        meleeEnemy_.getX() + meleeEnemy_.getHitboxWidth() / 2.0F - halfPlatformWidth,
        upperPlatformY,
        UPPER_PLATFORM_WIDTH,
        UPPER_PLATFORM_THICKNESS});
    platformSystem_.addPlatform(Rectangle{
        rangedEnemy_.getX() + rangedEnemy_.getHitboxWidth() / 2.0F - halfPlatformWidth,
        upperPlatformY,
        UPPER_PLATFORM_WIDTH,
        UPPER_PLATFORM_THICKNESS});
}

bool GameScene::load() {
    // VS Code 从项目根目录启动；复制后的 build/assets 则支持直接在 build 中运行。
    if (playerSpriteRenderer_.load("assets/移动.png")) return true;

    const std::string executableRelativePath =
        std::string(GetApplicationDirectory()) + "assets/移动.png";
    return playerSpriteRenderer_.load(executableRelativePath.c_str());
}

Rectangle GameScene::makeCharacterHitbox(
    const Character& character, const Rectangle& platform) {
    return Rectangle{
        character.getX(),
        platform.y - character.getHitboxHeight() - character.getY(),
        character.getHitboxWidth(),
        character.getHitboxHeight()};
}

void GameScene::showDamageText(float damage, float textX, float textY) {
    displayedDamage_ = damage;
    damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
    damageTextX_ = textX;
    damageTextY_ = textY;
}

void GameScene::update(float deltaTime) {
    // Scene 只读取设备无关状态，不直接判断键盘或手柄的具体按键。
    const PlayerInputState input = playerController_.pollInput();
    const HorizontalInputDirection horizontalDirection =
        PlayerController::resolveHorizontalDirection(input);
    PlayerShadow* synchronizedShadow = shadowManager_.getShadow();
    if (input.useShadowSkill2Pressed && synchronizedShadow != nullptr) {
        synchronizedShadow->tryUsePositionSwap(player_);
    }
    if (input.selectShadowSkill1Pressed && synchronizedShadow != nullptr) {
        // 测试阶段直接切换当前影子的技能；技能状态由 PlayerShadow 自己保存。
        synchronizedShadow->setActiveSkill(ShadowSkill::SynchronizePlayerActions);
    }
    const bool attackPressed =
        player_.applyInput(input, horizontalDirection, deltaTime);
    const bool shadowAttackPressed = synchronizedShadow != nullptr ? synchronizedShadow->applyInput(input, horizontalDirection, deltaTime) : false;

    // 所有角色共用平台支撑、实体更新、小平台落地和主地面落地顺序。
    const auto updateCharacterPhysics = [&](Character& character) {
        platformSystem_.updateCharacterSupport(character, platform_.y);
        const float previousHeight = character.getY();
        character.update(deltaTime);
        // 闪避等在 update 内发生的水平位移也可能让角色离开平台。
        platformSystem_.updateCharacterSupport(character, platform_.y);
        platformSystem_.resolveCharacterLanding(
            character, previousHeight, platform_.y);
        if (character.getY() <= 0.0F) {
            character.land();
        }
    };
    updateCharacterPhysics(player_);
    updateCharacterPhysics(meleeEnemy_);
    updateCharacterPhysics(rangedEnemy_);

    const float maxPlayerX =
        platform_.x + platform_.width - player_.getHitboxWidth();
    player_.setPosition(
        std::clamp(player_.getX(), platform_.x, maxPlayerX), player_.getY());
    // 伤害数字只保留一秒，与影子是否已经消失无关。
    if (damageTextElapsedTime_ > 0.0F) {
        damageTextElapsedTime_ = std::max(0.0F, damageTextElapsedTime_ - deltaTime);
    }
    if (attackEffectElapsedTime_ > 0.0F) {
        attackEffectElapsedTime_ =
            std::max(0.0F, attackEffectElapsedTime_ - deltaTime);
    }
    if (shadowAttackEffectElapsedTime_ > 0.0F) {
        shadowAttackEffectElapsedTime_ =
            std::max(0.0F, shadowAttackEffectElapsedTime_ - deltaTime);
    }
    if (meleeAttackEffectElapsedTime_ > 0.0F) {
        meleeAttackEffectElapsedTime_ =
            std::max(0.0F, meleeAttackEffectElapsedTime_ - deltaTime);
    }
    if (attackPressed) {
        attackEffectElapsedTime_ = ATTACK_EFFECT_LIFETIME;
    }
    if (shadowAttackPressed) {
        shadowAttackEffectElapsedTime_ = ATTACK_EFFECT_LIFETIME;
    }

    const auto spawnFriendlyProjectile = [&](Player& attacker) {
        if (!attacker.consumeRangedAttackRequest()) return;

        const float direction = attacker.isFacingRight() ? 1.0F : -1.0F;
        const float projectileX = attacker.isFacingRight() ? attacker.getX() + attacker.getHitboxWidth() + PROJECTILE_RADIUS : attacker.getX() - PROJECTILE_RADIUS;
        projectileSystem_.spawn(ProjectileSystem::SpawnInfo{
            projectileX,
            platform_.y - attacker.getHitboxHeight() / 2.0F - attacker.getY(),
            direction,
            PROJECTILE_SPEED,
            PROJECTILE_MAX_DISTANCE,
            PROJECTILE_RADIUS,
            attacker.getAttackDamage(),
            Faction::Friendly,
            ORANGE,
            0});
    };
    spawnFriendlyProjectile(player_);
    if (synchronizedShadow != nullptr) {
        spawnFriendlyProjectile(*synchronizedShadow);
    }

    const CombatSystem::AttackResult enemyMeleeResult = combatSystem_.enemyMeleeAttack(
        meleeEnemy_, player_, platform_.y);
    if (enemyMeleeResult.attackPerformed) {
        meleeAttackEffectElapsedTime_ = ATTACK_EFFECT_LIFETIME;
    }

    Character* rangedEnemyTarget = combatSystem_.findNearestEnemyTarget(
        rangedEnemy_, player_);
    if (rangedEnemyTarget != nullptr) {
        // 朝向、子弹出生侧和飞行方向都使用同一个目标判断结果。
        rangedEnemy_.faceToward(*rangedEnemyTarget);
        if (rangedEnemy_.tryAttack()) {
            const float direction = rangedEnemy_.isFacingRight() ? 1.0F : -1.0F;
            const float projectileX = rangedEnemy_.isFacingRight() ? rangedEnemy_.getX() + rangedEnemy_.getHitboxWidth() + PROJECTILE_RADIUS : rangedEnemy_.getX() - PROJECTILE_RADIUS;
            projectileSystem_.spawn(ProjectileSystem::SpawnInfo{
                projectileX,
                platform_.y - rangedEnemy_.getHitboxHeight() / 2.0F -
                    rangedEnemy_.getY(),
                direction,
                PROJECTILE_SPEED,
                PROJECTILE_MAX_DISTANCE,
                PROJECTILE_RADIUS,
                rangedEnemy_.getAttackDamage(),
                Faction::Enemy,
                Color{255, 45, 20, 255},
                0});
        }
    }

    // Scene 只组装本帧目标；弹道筛选阵营、移动、命中和销毁均由系统处理。
    std::vector<ProjectileSystem::Target> projectileTargets;
    ProjectileSystem::Target playerTarget{
        &player_,
        makeCharacterHitbox(player_, platform_),
        player_.isDodging(),
        player_.isDefending(),
        combatSystem_.makePlayerDefenseHitbox(player_, platform_.y),
        [&]() { return player_.blockNextAttack(); }};
    projectileTargets.push_back(playerTarget);
    // 影子没有受击判定，不加入弹道目标列表；所有弹道都会直接穿过。
    projectileTargets.push_back(ProjectileSystem::Target{
        &meleeEnemy_,
        makeCharacterHitbox(meleeEnemy_, platform_),
        false, false, Rectangle{}, {}});
    projectileTargets.push_back(ProjectileSystem::Target{
        &rangedEnemy_,
        makeCharacterHitbox(rangedEnemy_, platform_),
        false, false, Rectangle{}, {}});

    const std::vector<ProjectileSystem::Impact> projectileImpacts =
        projectileSystem_.update(deltaTime, projectileTargets);
    for (const ProjectileSystem::Impact& impact : projectileImpacts) {
        if (impact.blocked) continue;
        showDamageText(impact.damage, impact.textX, impact.textY);
    }

    if (!player_.isAlive()) {
        // 玩家死亡后立即在出生点回满血复活，并重新开始影子距离累计。
        player_.heal(player_.getMaxHealth());
        player_.setPosition(playerSpawnX_, playerSpawnY_);
        player_.land();
        shadowManager_.resetPlayerTracking(player_);
    }

    if (attackPressed) {
        const CombatSystem::AttackResult playerAttackResult = combatSystem_.playerMeleeAttack(
            player_, {&meleeEnemy_, &rangedEnemy_}, platform_.y);
        if (playerAttackResult.hit) {
            showDamageText(
                playerAttackResult.damage,
                playerAttackResult.targetHitbox.x +
                    playerAttackResult.targetHitbox.width / 2.0F,
                playerAttackResult.targetHitbox.y - 20.0F);
        }
    }
    if (shadowAttackPressed && synchronizedShadow != nullptr) {
        const CombatSystem::AttackResult shadowAttackResult = combatSystem_.playerMeleeAttack(
            *synchronizedShadow, {&meleeEnemy_, &rangedEnemy_}, platform_.y);
        if (shadowAttackResult.hit) {
            showDamageText(
                shadowAttackResult.damage,
                shadowAttackResult.targetHitbox.x +
                    shadowAttackResult.targetHitbox.width / 2.0F,
                shadowAttackResult.targetHitbox.y - 20.0F);
        }
    }

    const auto awardEnemyExperience = [&](const Enemy& enemy, bool& experienceAwarded) {
        // 经验标记防止敌人 5 秒尸体期的每一帧都重复加经验。
        if (enemy.isAlive()) {
            experienceAwarded = false;
            return;
        }
        if (!experienceAwarded) {
            // Scene 不计算经验公式，只领取当前兵种自身配置的击杀奖励。
            player_.addExperience(enemy.getExperienceReward());
            experienceAwarded = true;
        }
    };
    awardEnemyExperience(meleeEnemy_, meleeEnemyExperienceAwarded_);
    awardEnemyExperience(rangedEnemy_, rangedEnemyExperienceAwarded_);

    // ShadowManager 负责生命周期，平台系统只负责实体支撑和落地。
    PlayerShadow* shadowBeforeUpdate = shadowManager_.getShadow();
    const bool hadShadowBeforeUpdate = shadowBeforeUpdate != nullptr;
    const float previousShadowHeight = hadShadowBeforeUpdate ? shadowBeforeUpdate->getY() : 0.0F;
    if (shadowBeforeUpdate != nullptr) {
        platformSystem_.updateCharacterSupport(*shadowBeforeUpdate, platform_.y);
    }
    shadowManager_.update(player_, deltaTime);
    if (PlayerShadow* shadowAfterUpdate = shadowManager_.getShadow();
        shadowAfterUpdate != nullptr) {
        platformSystem_.updateCharacterSupport(*shadowAfterUpdate, platform_.y);
        const float heightBeforePhysics = hadShadowBeforeUpdate ? previousShadowHeight : shadowAfterUpdate->getY();
        platformSystem_.resolveCharacterLanding(
            *shadowAfterUpdate, heightBeforePhysics, platform_.y);
        const float maxShadowX =
            platform_.x + platform_.width - shadowAfterUpdate->getHitboxWidth();
        shadowAfterUpdate->setPosition(
            std::clamp(shadowAfterUpdate->getX(), platform_.x, maxShadowX),
            shadowAfterUpdate->getY());
    }
}

void GameScene::draw() const {
    ClearBackground(Color{24, 28, 38, 255});

    DrawRectangleRec(platform_, Color{126, 91, 67, 255});
    platformSystem_.draw(Color{126, 91, 67, 255});
    shadowManager_.draw(platform_.y, playerSpriteRenderer_);
    characterRenderer_.draw(
        player_,
        meleeEnemy_,
        rangedEnemy_,
        shadowManager_.getShadow(),
        platform_.y,
        playerSpriteRenderer_,
        CharacterEffectView{
            attackEffectElapsedTime_ > 0.0F,
            shadowAttackEffectElapsedTime_ > 0.0F,
            meleeAttackEffectElapsedTime_ > 0.0F,
            combatSystem_.getAttackRange(),
            combatSystem_.getDefenseRange()});
    projectileSystem_.draw();
    hudRenderer_.draw(
        player_,
        DamageTextView{
            damageTextElapsedTime_ > 0.0F,
            displayedDamage_,
            damageTextX_,
            damageTextY_});
}
