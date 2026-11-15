// Player.cpp
#include "Player.h"
#include <cmath>
#include <stdexcept>

Player::Player(sf::Vector2f startPosition, float speed, float size)
    : position_(startPosition), speed_(speed), size_(size) {
    if (speed_ <= 0.f || size_ <= 0.f) {
        throw std::invalid_argument("Player: speed and size must be positive");
    }
}

sf::FloatRect Player::boundsAt(sf::Vector2f centre) const {
    return sf::FloatRect(centre.x - size_ / 2.f, centre.y - size_ / 2.f, size_, size_);
}

sf::FloatRect Player::getBounds() const {
    return boundsAt(position_);
}

void Player::setSpeed(float speed) {
    if (speed <= 0.f) {
        throw std::invalid_argument("Player: speed must be positive");
    }
    speed_ = speed;
}

void Player::reset(sf::Vector2f position) {
    position_ = position;
}

void Player::move(sf::Vector2f direction, float dt, const CampusMap& map) {
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length == 0.f || dt <= 0.f) {
        return;
    }
    // normalise so moving diagonally is not faster
    const sf::Vector2f step(direction.x / length * speed_ * dt, direction.y / length * speed_ * dt);

    // try X and Y separately, so the player slides along walls
    const sf::Vector2f tryX(position_.x + step.x, position_.y);
    if (map.isWalkable(boundsAt(tryX))) {
        position_ = tryX;
    }
    const sf::Vector2f tryY(position_.x, position_.y + step.y);
    if (map.isWalkable(boundsAt(tryY))) {
        position_ = tryY;
    }
}

void Player::draw(sf::RenderTarget& target) const {
    sf::RectangleShape body(sf::Vector2f(size_, size_));
    body.setOrigin(size_ / 2.f, size_ / 2.f);
    body.setPosition(position_);
    body.setFillColor(sf::Color(220, 60, 60));
    body.setOutlineColor(sf::Color::Black);
    body.setOutlineThickness(2.f);
    target.draw(body);
}
