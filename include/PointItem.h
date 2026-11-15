// PointItem.h
// A normal collectible that gives points. Inherits from Collectible.
#pragma once
#include "Collectible.h"

class PointItem : public Collectible {
public:
    // Throws std::invalid_argument if points <= 0.
    PointItem(std::string name, sf::Vector2f position, int points);

    bool isRequired() const override { return true; }
    int getPoints() const { return points_; }

protected:
    void applyEffect(IGameContext& context) override;
    sf::Color getColor() const override { return sf::Color(255, 200, 40); }  // gold

private:
    int points_;
};
