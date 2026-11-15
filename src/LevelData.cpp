// LevelData.cpp
#include "LevelData.h"
#include "PointItem.h"
#include "TimeBonus.h"

namespace LevelData {

std::vector<std::unique_ptr<Collectible>> makeDefaultCollectibles() {
    std::vector<std::unique_ptr<Collectible>> items;

    // 8 point items (required to finish the round)
    items.push_back(std::make_unique<PointItem>("Library Book",   sf::Vector2f(510.f, 250.f), 10));
    items.push_back(std::make_unique<PointItem>("Lost ID Card",   sf::Vector2f(190.f, 250.f), 10));
    items.push_back(std::make_unique<PointItem>("Chai Cup",       sf::Vector2f(840.f, 250.f),  5));
    items.push_back(std::make_unique<PointItem>("Laptop",         sf::Vector2f(180.f, 520.f), 20));
    items.push_back(std::make_unique<PointItem>("Cricket Ball",   sf::Vector2f(820.f, 430.f), 15));
    items.push_back(std::make_unique<PointItem>("Notebook",       sf::Vector2f(380.f, 420.f),  5));
    items.push_back(std::make_unique<PointItem>("Water Bottle",   sf::Vector2f(300.f, 640.f),  5));
    items.push_back(std::make_unique<PointItem>("Calculator",     sf::Vector2f(740.f, 640.f), 10));

    // 2 time bonuses (optional)
    items.push_back(std::make_unique<TimeBonus>("Stopwatch",      sf::Vector2f(580.f, 500.f), 10.f));
    items.push_back(std::make_unique<TimeBonus>("Energy Drink",   sf::Vector2f(900.f, 640.f),  8.f));

    return items;
}

}  // namespace LevelData
