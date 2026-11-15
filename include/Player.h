// Player.h
// The character controlled by the user. Position and speed are private;
// the only way to move is through move(), which checks collisions with the map.
#pragma once
#include <SFML/Graphics.hpp>
#include "CampusMap.h"

class Player {
public:
    // position = centre of the player. Throws std::invalid_argument for speed <= 0 or size <= 0.
    Player(sf::Vector2f startPosition, float speed = 200.f, float size = 24.f);

    // direction: (-1..1, -1..1). dt: seconds since last frame.
    // Moves each axis separately so the player slides along walls instead of sticking.
    void move(sf::Vector2f direction, float dt, const CampusMap& map);

    void reset(sf::Vector2f position);
    void draw(sf::RenderTarget& target) const;

    sf::Vector2f getPosition() const { return position_; }
    sf::FloatRect getBounds() const;
    float getSpeed() const { return speed_; }
    void setSpeed(float speed);   // throws std::invalid_argument if speed <= 0

private:
    sf::FloatRect boundsAt(sf::Vector2f centre) const;

    sf::Vector2f position_;
    float speed_;
    float size_;
};
