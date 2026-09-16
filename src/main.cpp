#include "engine/game_engine.h"
#include "scenes/game_scene.h"

#include <memory>

int main() {
    GameEngine engine(std::make_unique<GameScene>());
    return engine.run();
}
