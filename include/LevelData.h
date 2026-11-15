// LevelData.h
// Data for the first level: where the player starts and where every item is placed.
// Keeping this in one place makes it easy to change the level without touching the classes.
#pragma once
#include <memory>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "Collectible.h"

namespace LevelData {
    constexpr float ROUND_SECONDS = 90.f;
    const sf::Vector2f PLAYER_START(512.f, 670.f);

    // Creates the 8 point items + 2 time bonuses. Ownership goes to the caller.
    std::vector<std::unique_ptr<Collectible>> makeDefaultCollectibles();
}
