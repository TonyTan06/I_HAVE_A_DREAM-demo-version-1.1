#pragma once

#include "entities/enemy.h"
#include <string>

class MeleeEnemy : public Enemy {
protected:
    explicit MeleeEnemy(std::string name);
};
