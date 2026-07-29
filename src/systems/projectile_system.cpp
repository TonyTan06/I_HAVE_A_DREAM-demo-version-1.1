#include "systems/projectile_system.h"

#include <algorithm>
#include <cmath>

void ProjectileSystem::spawn(const SpawnInfo& spawnInfo) {
    // 无效尺寸或距离不会产生弹道，避免永不移动或无法销毁的对象进入系统。
    if (spawnInfo.speed <= 0.0F || spawnInfo.maxDistance <= 0.0F ||
        spawnInfo.radius <= 0.0F || spawnInfo.direction == 0.0F) return;

    Projectile projectile{
        spawnInfo,
        spawnInfo.x,
        spawnInfo.y,
        0.0F,
        std::max(0, spawnInfo.extraPenetrations),
        {}};
    projectiles_.push_back(projectile);
}

std::vector<ProjectileSystem::Impact> ProjectileSystem::update(
    float deltaTime, const std::vector<Target>& targets) {
    std::vector<Impact> impacts; // 返回给 Scene 的本帧命中结果
    const float safeDeltaTime = std::max(0.0F, deltaTime); // 负时间不允许弹道倒退

    for (auto projectileIterator = projectiles_.begin();
         projectileIterator != projectiles_.end();) {
        Projectile& projectile = *projectileIterator;
        const float frameDistance = projectile.properties.speed * safeDeltaTime;
        const float directionSign = projectile.properties.direction > 0.0F ? 1.0F : -1.0F;
        projectile.x += directionSign * frameDistance;
        projectile.travelledDistance += frameDistance;

        bool destroyProjectile = false; // 本帧结束前是否删除当前弹道
        for (const Target& target : targets) {
            Character* character = target.character;
            if (character == nullptr || target.invulnerable || !character->isAlive() ||
                character->getFaction() == projectile.properties.faction ||
                hasHitTarget(projectile, character)) continue;

            const Vector2 center{projectile.x, projectile.y};
            if (target.hasBlockHitbox && target.tryBlock &&
                CheckCollisionCircleRec(center, projectile.properties.radius, target.blockHitbox) &&
                target.tryBlock()) {
                impacts.push_back(Impact{nullptr, 0.0F, 0.0F, 0.0F, true});
                destroyProjectile = true;
                break;
            }

            if (!CheckCollisionCircleRec(center, projectile.properties.radius, target.hitbox)) {
                continue;
            }

            character->takeDamage(projectile.properties.damage);
            projectile.hitTargets.push_back(character);
            impacts.push_back(Impact{
                character,
                projectile.properties.damage,
                target.hitbox.x + target.hitbox.width / 2.0F,
                target.hitbox.y - 20.0F,
                false});

            if (projectile.remainingPenetrations <= 0) {
                destroyProjectile = true;
                break;
            }
            --projectile.remainingPenetrations;
        }

        if (destroyProjectile ||
            projectile.travelledDistance >= projectile.properties.maxDistance) {
            projectileIterator = projectiles_.erase(projectileIterator);
        } else {
            ++projectileIterator;
        }
    }

    return impacts;
}

void ProjectileSystem::draw() const {
    for (const Projectile& projectile : projectiles_) {
        DrawCircle(static_cast<int>(projectile.x), static_cast<int>(projectile.y),
                   projectile.properties.radius, projectile.properties.color);
    }
}

std::size_t ProjectileSystem::getProjectileCount() const {
    return projectiles_.size();
}

bool ProjectileSystem::hasHitTarget(const Projectile& projectile, const Character* target) {
    return std::find(projectile.hitTargets.begin(), projectile.hitTargets.end(), target) !=
        projectile.hitTargets.end();
}
