#pragma once

#include "engine/scene.h"
#include "entities/melee_enemy.h"
#include "entities/player.h"
#include "entities/player_shadow.h"
#include "entities/ranged_enemy.h"
#include "input/player_controller.h"
#include "rendering/character_renderer.h"
#include "rendering/hud_renderer.h"
#include "rendering/player_sprite_renderer.h"
#include "raylib.h"
#include "systems/combat_system.h"
#include "systems/platform_system.h"
#include "systems/projectile_system.h"
#include "systems/shadow_manager.h"

class GameScene final : public Scene {
public:
    GameScene(); // 创建玩家、敌军、主平台、头顶平台以及各功能系统

    bool load() override; // raylib 窗口创建后加载玩家双帧素材
    void update(float deltaTime) override; // 每帧调用各实体和系统
    void draw() const override; // 组合关卡、角色、弹道与 HUD 的绘制顺序

private:
    Player player_; // 受键盘控制的主角实体
    PlayerSpriteRenderer playerSpriteRenderer_; // 加载并绘制玩家与影子的站立/行走双帧素材
    CharacterRenderer characterRenderer_; // 绘制角色本体、状态条与近战特效
    HudRenderer hudRenderer_; // 绘制操作说明、等级、经验与伤害数字
    PlayerController playerController_; // 将键盘及后续输入设备转换为统一玩家操作
    MeleeEnemy meleeEnemy_; // 平台右侧生成的近战敌军
    RangedEnemy rangedEnemy_; // 平台右侧生成的远程敌军
    float playerSpawnX_; // 玩家死亡后复活使用的出生点 x 坐标
    float playerSpawnY_; // 玩家死亡后复活使用的出生点 y 坐标
    ShadowManager shadowManager_; // 只管理影子记录点、生成距离、生命周期和销毁
    CombatSystem combatSystem_; // 统一管理近战目标选择、攻击框、防御与伤害结算
    Rectangle platform_; // 平台的位置与尺寸，同时定义地面高度
    PlatformSystem platformSystem_; // 管理敌军头顶的可站立单向平台
    float damageTextElapsedTime_; // 最近一次伤害数字还应显示多久，单位：秒
    float damageTextX_; // 最近一次伤害数字的绘制 x 坐标
    float damageTextY_; // 最近一次伤害数字的绘制 y 坐标
    float displayedDamage_; // 最近一次命中的伤害数值
    float attackEffectElapsedTime_; // 玩家白色近战特效的剩余显示时间
    float shadowAttackEffectElapsedTime_; // 技能 1 影子白色近战特效的剩余显示时间
    float meleeAttackEffectElapsedTime_; // 敌军粉色近战特效的剩余显示时间
    bool meleeEnemyExperienceAwarded_; // 本次近战兵死亡是否已经奖励过经验
    bool rangedEnemyExperienceAwarded_; // 本次远程兵死亡是否已经奖励过经验
    ProjectileSystem projectileSystem_; // 统一管理生成、移动、碰撞、伤害与绘制的弹道系统

    static constexpr float DAMAGE_TEXT_LIFETIME = 1.0F; // 伤害数字显示时长
    static constexpr float ATTACK_EFFECT_LIFETIME = 0.15F; // 一次刀刃特效显示时长
    static constexpr float PROJECTILE_RADIUS = 4.0F; // 当前普通远程攻击的圆形子弹半径
    static constexpr float PROJECTILE_SPEED = 300.0F; // 当前普通远程攻击的子弹速度
    static constexpr float PROJECTILE_MAX_DISTANCE = 500.0F; // 当前普通远程攻击的最大距离
    static constexpr float ENEMY_SPACING = 100.0F; // 两个敌军生成位置的水平间隔
    static constexpr float RANGED_ENEMY_SPAWN_X = 768.0F; // 远程兵原有的固定出生点，不随平台宽度变化
    static constexpr float UPPER_PLATFORM_WIDTH = 100.0F; // 敌军头顶平台的水平长度
    static constexpr float UPPER_PLATFORM_THICKNESS = 5.0F; // 敌军头顶平台的绘制厚度
    static constexpr float UPPER_PLATFORM_HEIGHT = 60.0F; // 小平台顶面到地面的垂直高度

    // 根据角色的逻辑坐标和平台地面位置，统一创建屏幕空间中的角色碰撞箱。
    static Rectangle makeCharacterHitbox(
        const Character& character, const Rectangle& platform);
    // 统一设置最近一次伤害数字的内容、位置和显示时间。
    void showDamageText(float damage, float textX, float textY);
};
