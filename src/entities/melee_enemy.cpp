#include "entities/melee_enemy.h"

#include "entities/player.h"

MeleeEnemy::MeleeEnemy(std::string name)
    : Enemy(std::move(name)) {
        
    rangedAttackRange_ = 0.0F; // 近战兵种没有远程攻击能力
}
