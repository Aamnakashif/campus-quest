// PointItem.cpp
#include "PointItem.h"
#include <stdexcept>
#include <utility>

PointItem::PointItem(std::string name, sf::Vector2f position, int points)
    : Collectible(std::move(name), position), points_(points) {
    if (points_ <= 0) {
        throw std::invalid_argument("PointItem: points must be positive");
    }
}

void PointItem::applyEffect(IGameContext& context) {
    context.addScore(points_);
    context.showMessage(getName() + " +" + std::to_string(points_) + " points");
}
