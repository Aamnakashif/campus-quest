// CampusMap.cpp
#include "CampusMap.h"

CampusMap::CampusMap(const sf::FloatRect& bounds) : bounds_(bounds) {}

void CampusMap::addObstacle(const sf::FloatRect& rect) {
    obstacles_.push_back(rect);
}

void CampusMap::addLandmark(const Landmark& landmark) {
    landmarks_.push_back(landmark);
    if (landmark.blocksMovement) {
        obstacles_.push_back(landmark.area);
    }
}

bool CampusMap::isWalkable(const sf::FloatRect& area) const {
    const bool inside = area.left >= bounds_.left &&
                        area.top >= bounds_.top &&
                        area.left + area.width <= bounds_.left + bounds_.width &&
                        area.top + area.height <= bounds_.top + bounds_.height;
    if (!inside) {
        return false;
    }
    for (const sf::FloatRect& obstacle : obstacles_) {
        if (obstacle.intersects(area)) {
            return false;
        }
    }
    return true;
}

CampusMap CampusMap::createDefaultCampus() {
    // Map covers the window below the 48 px HUD bar: x 0..1024, y 48..768
    CampusMap map(sf::FloatRect(0.f, 48.f, 1024.f, 720.f));

    // Landmarks (simplified placeholders, not exact copies of the real campus)
    map.addLandmark({"Admin Block",    sf::FloatRect( 80.f, 100.f, 220.f, 120.f), sf::Color(190, 120,  90), true});
    map.addLandmark({"Library",        sf::FloatRect(380.f, 100.f, 260.f, 120.f), sf::Color(120, 130, 200), true});
    map.addLandmark({"Cafeteria",      sf::FloatRect(740.f, 100.f, 200.f, 120.f), sf::Color(220, 150,  70), true});
    map.addLandmark({"EE Department",  sf::FloatRect( 80.f, 330.f, 200.f, 140.f), sf::Color(110, 170, 160), true});
    map.addLandmark({"Sports Ground",  sf::FloatRect(700.f, 330.f, 260.f, 200.f), sf::Color( 90, 170,  90), false});
    map.addLandmark({"Main Gate",      sf::FloatRect(432.f, 708.f, 160.f,  60.f), sf::Color(150, 150, 150), false});

    // Plain obstacles
    map.addObstacle(sf::FloatRect(470.f, 360.f,  80.f,  80.f));   // fountain
    map.addObstacle(sf::FloatRect(330.f, 540.f, 200.f,  20.f));   // hedge
    map.addObstacle(sf::FloatRect(640.f, 560.f,  20.f, 120.f));   // hedge
    return map;
}

void CampusMap::draw(sf::RenderTarget& target, const sf::Font& font) const {
    // grass
    sf::RectangleShape ground(sf::Vector2f(bounds_.width, bounds_.height));
    ground.setPosition(bounds_.left, bounds_.top);
    ground.setFillColor(sf::Color(110, 190, 110));
    target.draw(ground);

    // obstacles (hedges, fountain, building footprints)
    for (const sf::FloatRect& o : obstacles_) {
        sf::RectangleShape r(sf::Vector2f(o.width, o.height));
        r.setPosition(o.left, o.top);
        r.setFillColor(sf::Color(40, 110, 60));
        target.draw(r);
    }

    // landmarks painted over their footprints, with a name label
    for (const Landmark& lm : landmarks_) {
        sf::RectangleShape r(sf::Vector2f(lm.area.width, lm.area.height));
        r.setPosition(lm.area.left, lm.area.top);
        r.setFillColor(lm.color);
        r.setOutlineColor(sf::Color(40, 40, 40));
        r.setOutlineThickness(2.f);
        target.draw(r);

        sf::Text label(lm.name, font, 18);
        label.setFillColor(sf::Color::White);
        label.setOutlineColor(sf::Color::Black);
        label.setOutlineThickness(1.f);
        const sf::FloatRect tb = label.getLocalBounds();
        label.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        label.setPosition(lm.area.left + lm.area.width / 2.f, lm.area.top + lm.area.height / 2.f);
        target.draw(label);
    }
}
