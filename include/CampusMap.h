// CampusMap.h
// Stores the walkable area, obstacle rectangles and named landmarks of the campus.
// The art is placeholder rectangles for now; real artwork can replace it later.
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using RectList = std::vector<sf::FloatRect>;

struct Landmark {
    std::string name;
    sf::FloatRect area;
    sf::Color color;
    bool blocksMovement;   // buildings block the player, the gate and sports ground do not
};

class CampusMap {
public:
    explicit CampusMap(const sf::FloatRect& bounds);

    // Builds the simplified COMSATS-style layout used in the game.
    static CampusMap createDefaultCampus();

    void addObstacle(const sf::FloatRect& rect);     // hedges, fountain, ...
    void addLandmark(const Landmark& landmark);      // blocking landmarks also become obstacles

    // True when the whole rectangle is inside the map and touches no obstacle.
    bool isWalkable(const sf::FloatRect& area) const;

    const sf::FloatRect& getBounds() const { return bounds_; }
    const RectList& getObstacles() const { return obstacles_; }
    const std::vector<Landmark>& getLandmarks() const { return landmarks_; }

    void draw(sf::RenderTarget& target, const sf::Font& font) const;

private:
    sf::FloatRect bounds_;
    RectList obstacles_;
    std::vector<Landmark> landmarks_;
};
