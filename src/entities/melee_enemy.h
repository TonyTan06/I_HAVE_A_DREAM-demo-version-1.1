#pragma once

#include "entities/enemy.h"

class Player;

class MeleeEnemy : public Enemy {
public:
    explicit MeleeEnemy(const Player& player, int level = 1);
};
