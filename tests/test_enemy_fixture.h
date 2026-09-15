#pragma once

#include "entities/enemy.h"

// 只用于验证 Enemy 基类与系统接口，不作为游戏中的具体敌人类型。
class TestEnemy final : public Enemy {
public:
    TestEnemy()
        : Enemy("Test Enemy") {
        health_ = 2;
        setHitboxSize(20.0F, 40.0F);
        detectionRange_ = 200.0F;
        meleeAttackRange_ = 16.0F;
        rangedAttackRange_ = 200.0F;
    }

    void setDetectionRangeForTest(float range) {
        detectionRange_ = range;
    }
    void setMeleeRangeForTest(float range) {
        meleeAttackRange_ = range;
    }
    void setRangedRangeForTest(float range) {
        rangedAttackRange_ = range;
    }
};
