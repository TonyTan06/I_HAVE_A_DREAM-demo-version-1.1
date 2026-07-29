#pragma once

#include "entities/enemy.h"

class Player;

class RangedEnemy : public Enemy {
public:
    explicit RangedEnemy(const Player& player, int level = 1);
};
