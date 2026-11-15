// Collectible.cpp
#include "Collectible.h"
#include <stdexcept>
#include <utility>

Collectible::Collectible(std::string name, sf::Vector2f position, float radius)
    : name_(std::move(name)), position_(position), radius_(radius), collected_(false) {
    if (radius_ <= 0.f) {
        throw std::invalid_argument("Collectible: radius must be positive");
    }
}

bool Collectible::collect(IGameContext& context) {
    if (collected_) {
        return false;          // an item can never give its reward twice
    }
    collected_ = true;
    applyEffect(context);      // polymorphic call: PointItem or TimeBonus behaviour
    return true;
}

sf::FloatRect Collectible::getBounds() const {
    return sf::FloatRect(position_.x - radius_, position_.y - radius_, radius_ * 2.f, radius_ * 2.f);
}

void Collectible::draw(sf::RenderTarget& target) const {
    if (collected_) {
        return;
    }
    sf::CircleShape shape(radius_);
    shape.setOrigin(radius_, radius_);
    shape.setPosition(position_);
    shape.setFillColor(getColor());
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.f);
    target.draw(shape);
}
