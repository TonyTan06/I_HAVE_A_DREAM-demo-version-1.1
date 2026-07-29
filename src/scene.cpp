#include "scene.h"

#include <algorithm>
#include <string>
#include <vector>

Scene::Scene()
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

bool Scene::loadAssets() {
    // VS Code 从项目根目录启动；复制后的 build/assets 则支持直接在 build 中运行。
    if (playerSpriteRenderer_.load("assets/移动.png")) return true;

    const std::string executableRelativePath =
        std::string(GetApplicationDirectory()) + "assets/移动.png";
    return playerSpriteRenderer_.load(executableRelativePath.c_str());
}

Rectangle Scene::makeCharacterHitbox(
    const Character& character, const Rectangle& platform) {
    return Rectangle{
        character.getX(),
        platform.y - character.getHitboxHeight() - character.getY(),
        character.getHitboxWidth(),
        character.getHitboxHeight()};
}

void Scene::drawBackVerticalCooldownBar(
    const Character& character, bool facingRight,
    float characterTop, float cooldownProgress) {
    const float clampedProgress = std::clamp(cooldownProgress, 0.0F, 1.0F);
    const int barX = static_cast<int>(facingRight
        ? character.getX() - 6.0F
        : character.getX() + character.getHitboxWidth() + 2.0F);
    const int barY = static_cast<int>(characterTop);
    const int barHeight = static_cast<int>(character.getHitboxHeight());
    const int progressHeight = static_cast<int>(barHeight * clampedProgress);

    DrawRectangle(barX, barY, 4, barHeight, DARKGRAY);
    DrawRectangle(barX, barY + barHeight - progressHeight,
                  4, progressHeight, GREEN);
}

void Scene::showDamageText(float damage, float textX, float textY) {
    displayedDamage_ = damage;
    damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
    damageTextX_ = textX;
    damageTextY_ = textY;
}

void Scene::update(float deltaTime) {
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
    const bool shadowAttackPressed = synchronizedShadow != nullptr
        ? synchronizedShadow->applyInput(input, horizontalDirection, deltaTime)
        : false;

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
        const float projectileX = attacker.isFacingRight()
            ? attacker.getX() + attacker.getHitboxWidth() + PROJECTILE_RADIUS
            : attacker.getX() - PROJECTILE_RADIUS;
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
            const float projectileX = rangedEnemy_.isFacingRight()
                ? rangedEnemy_.getX() + rangedEnemy_.getHitboxWidth() + PROJECTILE_RADIUS
                : rangedEnemy_.getX() - PROJECTILE_RADIUS;
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
    const float previousShadowHeight = hadShadowBeforeUpdate
        ? shadowBeforeUpdate->getY()
        : 0.0F;
    if (shadowBeforeUpdate != nullptr) {
        platformSystem_.updateCharacterSupport(*shadowBeforeUpdate, platform_.y);
    }
    shadowManager_.update(player_, deltaTime);
    if (PlayerShadow* shadowAfterUpdate = shadowManager_.getShadow();
        shadowAfterUpdate != nullptr) {
        platformSystem_.updateCharacterSupport(*shadowAfterUpdate, platform_.y);
        const float heightBeforePhysics = hadShadowBeforeUpdate
            ? previousShadowHeight
            : shadowAfterUpdate->getY();
        platformSystem_.resolveCharacterLanding(
            *shadowAfterUpdate, heightBeforePhysics, platform_.y);
        const float maxShadowX =
            platform_.x + platform_.width - shadowAfterUpdate->getHitboxWidth();
        shadowAfterUpdate->setPosition(
            std::clamp(shadowAfterUpdate->getX(), platform_.x, maxShadowX),
            shadowAfterUpdate->getY());
    }
}

void Scene::draw() const {
    ClearBackground(Color{24, 28, 38, 255});

    DrawRectangleRec(platform_, Color{126, 91, 67, 255});
    platformSystem_.draw(Color{126, 91, 67, 255});
    shadowManager_.draw(platform_.y, playerSpriteRenderer_);

    const auto drawEnemy = [&](const Enemy& enemy, Color eyeColor, bool facingRight) {
        // respawning 控制敌人尸体期的浅色身体和黄色复活倒计时条。
        const float enemyWidth = enemy.getHitboxWidth();
        const float enemyHeight = enemy.getHitboxHeight();
        const float enemyTop = platform_.y - enemyHeight - enemy.getY();
        const int barX = static_cast<int>(enemy.getX());
        const int barY = static_cast<int>(enemyTop - 10.0F);
        const bool respawning = !enemy.isAlive() || enemy.isRespawning();
        const Color bodyColor = respawning ? Color{235, 170, 170, 255}
                                           : Color{190, 90, 90, 255};
        const float barProgress = respawning
            ? 1.0F - enemy.getRespawnProgress()
            : enemy.getHealth() / enemy.getMaxHealth();

        DrawRectangle(static_cast<int>(enemy.getX()), static_cast<int>(enemyTop),
                      static_cast<int>(enemyWidth), static_cast<int>(enemyHeight), bodyColor);
        DrawRectangle(barX, barY, static_cast<int>(enemyWidth), 5, DARKGRAY);
        DrawRectangle(barX, barY,
                      static_cast<int>(enemyWidth * std::clamp(barProgress, 0.0F, 1.0F)),
                      5, respawning ? YELLOW : RED);
        const float eyeX = facingRight
            ? enemy.getX() + enemyWidth - 8.0F
            : enemy.getX() + 8.0F;
        DrawCircle(static_cast<int>(eyeX),
                   static_cast<int>(enemyTop + 14.0F), 3.0F, eyeColor);
    };
    drawEnemy(meleeEnemy_, BLUE, meleeEnemy_.isFacingRight());
    drawEnemy(rangedEnemy_, GREEN, rangedEnemy_.isFacingRight());
    playerSpriteRenderer_.draw(player_, platform_.y, WHITE);
    const float playerWidth = player_.getHitboxWidth();
    const float playerHeight = player_.getHitboxHeight();
    const float playerTop = platform_.y - playerHeight - player_.getY();
    if (player_.isDodgeCoolingDown()) {
        drawBackVerticalCooldownBar(
            player_, player_.isFacingRight(),
            playerTop, player_.getDodgeCooldownProgress());
    }
    const float playerHealthRatio = player_.getHealth() / player_.getMaxHealth();
    DrawRectangle(static_cast<int>(player_.getX()), static_cast<int>(playerTop - 10.0F),
                  static_cast<int>(playerWidth), 5, DARKGRAY);
    DrawRectangle(static_cast<int>(player_.getX()), static_cast<int>(playerTop - 10.0F),
                  static_cast<int>(playerWidth * std::clamp(playerHealthRatio, 0.0F, 1.0F)),
                  5, GREEN);
    const float eyeX = player_.isFacingRight()
        ? player_.getX() + playerWidth - 8.0F
        : player_.getX() + 8.0F;
    DrawCircle(static_cast<int>(eyeX), static_cast<int>(playerTop + 14.0F), 3.0F, BLACK);
    if (const PlayerShadow* shadow = shadowManager_.getShadow();
        shadow != nullptr && shadow->isPositionSwapCoolingDown()) {
        const float shadowTop =
            platform_.y - shadow->getHitboxHeight() - shadow->getY();
        drawBackVerticalCooldownBar(
            *shadow, shadow->isFacingRight(), shadowTop,
            shadow->getPositionSwapCooldownProgress());
    }
    if (player_.isDefending() || attackEffectElapsedTime_ > 0.0F) {
        const float bladeBaseX = player_.isFacingRight()
            ? player_.getX() + playerWidth
            : player_.getX();
        const Vector2 bladeTop{bladeBaseX, playerTop};
        const Vector2 bladeBottom{bladeBaseX, playerTop + playerHeight};
        const float bladeLength = player_.isDefending()
            ? combatSystem_.getDefenseRange()
            : combatSystem_.getAttackRange();
        const Vector2 bladeTip{
            player_.isFacingRight() ? bladeBaseX + bladeLength : bladeBaseX - bladeLength,
            playerTop + playerHeight / 2.0F};
        const Color bladeColor = player_.isDefending() ? Color{255, 222, 173, 255} : WHITE;
        if (player_.isFacingRight()) {
            DrawTriangle(bladeTop, bladeBottom, bladeTip, bladeColor);
        } else {
            // 左向时交换底边顶点，保持与右向一致的三角形顶点绕序。
            DrawTriangle(bladeBottom, bladeTop, bladeTip, bladeColor);
        }
    }
    if (const PlayerShadow* shadow = shadowManager_.getShadow();
        shadow != nullptr &&
        (shadow->isDefending() || shadowAttackEffectElapsedTime_ > 0.0F)) {
        const float shadowHeight = shadow->getHitboxHeight();
        const float shadowTop = platform_.y - shadowHeight - shadow->getY();
        const float bladeBaseX = shadow->isFacingRight()
            ? shadow->getX() + shadow->getHitboxWidth()
            : shadow->getX();
        const Vector2 bladeTop{bladeBaseX, shadowTop};
        const Vector2 bladeBottom{bladeBaseX, shadowTop + shadowHeight};
        const float bladeLength = shadow->isDefending()
            ? combatSystem_.getDefenseRange()
            : combatSystem_.getAttackRange();
        const Vector2 bladeTip{
            shadow->isFacingRight() ? bladeBaseX + bladeLength : bladeBaseX - bladeLength,
            shadowTop + shadowHeight / 2.0F};
        const Color bladeColor = shadow->isDefending()
            ? Color{255, 222, 173, 255}
            : WHITE;
        if (shadow->isFacingRight()) {
            DrawTriangle(bladeTop, bladeBottom, bladeTip, bladeColor);
        } else {
            DrawTriangle(bladeBottom, bladeTop, bladeTip, bladeColor);
        }
    }
    if (meleeAttackEffectElapsedTime_ > 0.0F) {
        const float enemyHeight = meleeEnemy_.getHitboxHeight();
        const float enemyTop = platform_.y - enemyHeight - meleeEnemy_.getY();
        const float bladeBaseX = meleeEnemy_.isFacingRight()
            ? meleeEnemy_.getX() + meleeEnemy_.getHitboxWidth()
            : meleeEnemy_.getX();
        const Vector2 bladeTop{bladeBaseX, enemyTop};
        const Vector2 bladeBottom{bladeBaseX, enemyTop + enemyHeight};
        const Vector2 bladeTip{
            meleeEnemy_.isFacingRight()
                ? bladeBaseX + combatSystem_.getAttackRange()
                : bladeBaseX - combatSystem_.getAttackRange(),
            enemyTop + enemyHeight / 2.0F};
        if (meleeEnemy_.isFacingRight()) {
            DrawTriangle(bladeTop, bladeBottom, bladeTip, Color{255, 182, 193, 255});
        } else {
            DrawTriangle(bladeBottom, bladeTop, bladeTip, Color{255, 182, 193, 255});
        }
    }
    projectileSystem_.draw();
    if (damageTextElapsedTime_ > 0.0F) {
        DrawText(TextFormat("%.0f", displayedDamage_), static_cast<int>(damageTextX_),
                 static_cast<int>(damageTextY_), 20, YELLOW);
    }
    DrawText("A/D or arrow keys: move    Space: jump    J: melee    K: ranged    U: defend    L: dodge    1: sync shadow    2: swap", 20, 20, 20,
             RAYWHITE);
    DrawText(TextFormat("Level: %d", player_.getLevel()), 20, 50, 20, RAYWHITE);
    DrawText(TextFormat("Exp: %d / %d", player_.getExperience(),
                        player_.getExperienceThreshold()),
             20, 76, 20, RAYWHITE);
}
