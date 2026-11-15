// Game.h
// Owns the window, runs the main loop and holds all game objects (composition).
// Implements IGameContext so collectibles can add score/time without knowing about Game.
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "IGameContext.h"
#include "Collectible.h"
#include "Player.h"
#include "CampusMap.h"
#include "ScoreManager.h"

enum class GameState { Menu, Playing, Paused, Results };

class Game : public IGameContext {
public:
    static constexpr unsigned WINDOW_WIDTH = 1024;
    static constexpr unsigned WINDOW_HEIGHT = 768;

    Game();                 // throws std::runtime_error if the font cannot be loaded
    void run();             // main loop: events -> update -> render

    // IGameContext
    void addScore(int points) override;
    void addTime(float seconds) override;
    void showMessage(const std::string& text) override;

    GameState getState() const { return state_; }
    float getTimeRemaining() const { return timeRemaining_; }

private:
    void processEvents();
    void update(float dt);
    void render();

    void startRound();
    void endRound(const std::string& reason);
    void updatePlaying(float dt);
    sf::Vector2f readDirection() const;

    void renderMenu();
    void renderWorld();
    void renderHud();
    void renderPaused();
    void renderResults();
    void drawCenteredText(const std::string& text, unsigned size, float y, sf::Color color);

    // --- members (composition: Game owns these) ---
    sf::RenderWindow window_;
    sf::Font font_;
    GameState state_;

    CampusMap map_;
    Player player_;
    ScoreManager scoreManager_;
    std::vector<std::unique_ptr<Collectible>> collectibles_;

    float timeRemaining_;
    int requiredFound_;
    int requiredTotal_;
    std::string message_;
    float messageTimer_;
    std::string endReason_;
};
