// Collectible.h
// Abstract base class for everything the player can pick up.
// Demonstrates: abstraction, encapsulation, and the base of the inheritance hierarchy.
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "IGameContext.h"

class Collectible {
public:
    Collectible(std::string name, sf::Vector2f position, float radius = 12.f);
    virtual ~Collectible() = default;

    // A collectible is a unique object in the world, so copying is not allowed.
    Collectible(const Collectible&) = delete;
    Collectible& operator=(const Collectible&) = delete;

    // Marks the item as collected and applies its effect ONCE.
    // Returns true only the first time it is called (Template Method pattern).
    bool collect(IGameContext& context);

    // Draws the item. Derived classes may override to add their own look.
    virtual void draw(sf::RenderTarget& target) const;

    // Does the round need this item to be collected to finish? (polymorphic)
    virtual bool isRequired() const = 0;

    // Getters (the data itself is private)
    const std::string& getName() const { return name_; }
    sf::Vector2f getPosition() const { return position_; }
    float getRadius() const { return radius_; }
    bool isCollected() const { return collected_; }
    sf::FloatRect getBounds() const;

protected:
    // What happens when the item is picked up. Each subclass decides.
    virtual void applyEffect(IGameContext& context) = 0;
    // Colour used when drawing the item.
    virtual sf::Color getColor() const = 0;

private:
    std::string name_;
    sf::Vector2f position_;   // centre of the item
    float radius_;
    bool collected_;
};
