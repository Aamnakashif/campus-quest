// TimeBonus.cpp
#include "TimeBonus.h"
#include <stdexcept>
#include <utility>

TimeBonus::TimeBonus(std::string name, sf::Vector2f position, float seconds)
    : Collectible(std::move(name), position), seconds_(seconds) {
    if (seconds_ <= 0.f) {
        throw std::invalid_argument("TimeBonus: seconds must be positive");
    }
}

void TimeBonus::applyEffect(IGameContext& context) {
    context.addTime(seconds_);
    context.showMessage(getName() + " +" + std::to_string(static_cast<int>(seconds_)) + " seconds");
}

void TimeBonus::draw(sf::RenderTarget& target) const {
    if (isCollected()) {
        return;
    }
    Collectible::draw(target);   // normal circle first

    // extra ring so time bonuses look different from point items
    const float ringRadius = getRadius() + 6.f;
    sf::CircleShape ring(ringRadius);
    ring.setOrigin(ringRadius, ringRadius);
    ring.setPosition(getPosition());
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineColor(getColor());
    ring.setOutlineThickness(2.f);
    target.draw(ring);
}
