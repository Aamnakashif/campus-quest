// TimeBonus.h
// A special collectible that adds seconds to the timer. Inherits from Collectible.
#pragma once
#include "Collectible.h"

class TimeBonus : public Collectible {
public:
    // Throws std::invalid_argument if seconds <= 0.
    TimeBonus(std::string name, sf::Vector2f position, float seconds);

    bool isRequired() const override { return false; }  // optional extra
    float getSeconds() const { return seconds_; }

    void draw(sf::RenderTarget& target) const override;  // adds a ring around the item

protected:
    void applyEffect(IGameContext& context) override;
    sf::Color getColor() const override { return sf::Color(80, 200, 255); }  // cyan

private:
    float seconds_;
};
