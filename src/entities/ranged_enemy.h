#pragma once

#include "entities/enemy.h"
#include <string>

class RangedEnemy : public Enemy {
protected:
    explicit RangedEnemy(std::string name); // 创建远程兵种
};
