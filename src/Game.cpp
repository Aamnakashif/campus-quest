// Game.cpp
#include "Game.h"
#include "LevelData.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <stdexcept>

namespace {

// Tries the project's font first, then common system fonts, so the game works
// on a fresh Windows machine (Arial is always installed there).
bool loadFontFromKnownLocations(sf::Font& font) {
    const char* candidates[] = {
        "assets/fonts/font.ttf",
        "../assets/fonts/font.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/Library/Fonts/Arial.ttf",
    };
    for (const char* path : candidates) {
        if (!std::filesystem::exists(path)) {
            continue;   // skip silently so SFML does not print errors for missing candidates
        }
        if (font.loadFromFile(path)) {
            return true;
        }
    }
    return false;
}

}  // namespace

Game::Game()
    : window_(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Campus Quest - A COMSATS Scavenger Hunt",
              sf::Style::Titlebar | sf::Style::Close),
      state_(GameState::Menu),
      map_(CampusMap::createDefaultCampus()),
      player_(LevelData::PLAYER_START),
      scoreManager_("best_score.txt"),
      timeRemaining_(LevelData::ROUND_SECONDS),
      requiredFound_(0),
      requiredTotal_(0),
      messageTimer_(0.f) {
    if (!loadFontFromKnownLocations(font_)) {
        throw std::runtime_error("Could not load a font. Put a .ttf file at assets/fonts/font.ttf");
    }
    window_.setFramerateLimit(60);
    scoreManager_.loadBestScore();
    if (!scoreManager_.getLastError().empty()) {
        showMessage("Warning: " + scoreManager_.getLastError());
    }
}

// ---------------------------------------------------------------- main loop
void Game::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        // clamp dt so a window drag or lag spike cannot teleport the player through walls
        const float dt = std::min(clock.restart().asSeconds(), 0.05f);
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
            return;
        }
        if (event.type == sf::Event::LostFocus && state_ == GameState::Playing) {
            state_ = GameState::Paused;   // pause automatically when the window loses focus
            continue;
        }
        if (event.type != sf::Event::KeyPressed) {
            continue;
        }
        const sf::Keyboard::Key key = event.key.code;
        switch (state_) {
            case GameState::Menu:
                if (key == sf::Keyboard::Enter) startRound();
                else if (key == sf::Keyboard::Escape) window_.close();
                break;
            case GameState::Playing:
                if (key == sf::Keyboard::P || key == sf::Keyboard::Escape) state_ = GameState::Paused;
                break;
            case GameState::Paused:
                if (key == sf::Keyboard::P || key == sf::Keyboard::Escape) state_ = GameState::Playing;
                else if (key == sf::Keyboard::M) state_ = GameState::Menu;
                break;
            case GameState::Results:
                if (key == sf::Keyboard::Enter) startRound();
                else if (key == sf::Keyboard::M || key == sf::Keyboard::Escape) state_ = GameState::Menu;
                break;
        }
    }
}

void Game::update(float dt) {
    if (state_ == GameState::Playing) {
        updatePlaying(dt);   // timer and movement only run while playing (pause freezes them)
    }
}

void Game::updatePlaying(float dt) {
    timeRemaining_ -= dt;
    if (messageTimer_ > 0.f) {
        messageTimer_ -= dt;
    }

    player_.move(readDirection(), dt, map_);

    for (const auto& item : collectibles_) {
        if (!item->isCollected() && player_.getBounds().intersects(item->getBounds())) {
            if (item->collect(*this) && item->isRequired()) {   // polymorphic call
                ++requiredFound_;
            }
        }
    }

    if (requiredFound_ >= requiredTotal_) {
        endRound("All items found!");
    } else if (timeRemaining_ <= 0.f) {
        timeRemaining_ = 0.f;
        endRound("Time's up!");
    }
}

sf::Vector2f Game::readDirection() const {
    sf::Vector2f dir(0.f, 0.f);
    if (!window_.hasFocus()) {
        return dir;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir.x += 1.f;
    return dir;
}

// ------------------------------------------------------------ round control
void Game::startRound() {
    collectibles_ = LevelData::makeDefaultCollectibles();
    requiredTotal_ = 0;
    for (const auto& item : collectibles_) {
        if (item->isRequired()) ++requiredTotal_;
    }
    requiredFound_ = 0;
    timeRemaining_ = LevelData::ROUND_SECONDS;
    scoreManager_.resetScore();
    player_.reset(LevelData::PLAYER_START);
    message_.clear();
    messageTimer_ = 0.f;
    state_ = GameState::Playing;
}

void Game::endRound(const std::string& reason) {
    endReason_ = reason;
    scoreManager_.saveBestIfNeeded();
    state_ = GameState::Results;
}

// ------------------------------------------------------- IGameContext (API)
void Game::addScore(int points) {
    scoreManager_.addPoints(points);
}

void Game::addTime(float seconds) {
    timeRemaining_ += seconds;
}

void Game::showMessage(const std::string& text) {
    message_ = text;
    messageTimer_ = 2.0f;
}

// ---------------------------------------------------------------- rendering
void Game::render() {
    window_.clear(sf::Color(25, 35, 60));
    switch (state_) {
        case GameState::Menu:    renderMenu(); break;
        case GameState::Playing: renderWorld(); renderHud(); break;
        case GameState::Paused:  renderWorld(); renderHud(); renderPaused(); break;
        case GameState::Results: renderResults(); break;
    }
    window_.display();
}

void Game::drawCenteredText(const std::string& text, unsigned size, float y, sf::Color color) {
    sf::Text t(text, font_, size);
    t.setFillColor(color);
    const sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, 0.f);
    t.setPosition(WINDOW_WIDTH / 2.f, y);
    window_.draw(t);
}

void Game::renderMenu() {
    drawCenteredText("CAMPUS QUEST", 72, 180.f, sf::Color(255, 200, 40));
    drawCenteredText("A COMSATS Scavenger Hunt", 28, 270.f, sf::Color::White);
    drawCenteredText("Press ENTER to start", 32, 390.f, sf::Color::White);
    drawCenteredText("Move: WASD / Arrow keys     Pause: P     Quit: ESC", 20, 450.f, sf::Color(180, 190, 210));
    drawCenteredText("Best score: " + std::to_string(scoreManager_.getBestScore()), 28, 530.f, sf::Color(120, 220, 140));
}

void Game::renderWorld() {
    map_.draw(window_, font_);
    for (const auto& item : collectibles_) {
        item->draw(window_);         // polymorphic: TimeBonus draws an extra ring
    }
    player_.draw(window_);
}

void Game::renderHud() {
    sf::RectangleShape bar(sf::Vector2f(static_cast<float>(WINDOW_WIDTH), 48.f));
    bar.setFillColor(sf::Color(20, 25, 40));
    window_.draw(bar);

    sf::Text score("Score: " + std::to_string(scoreManager_.getScore()), font_, 24);
    score.setPosition(16.f, 8.f);
    window_.draw(score);

    const int secs = static_cast<int>(std::ceil(timeRemaining_));
    sf::Text time("Time: " + std::to_string(secs) + "s", font_, 24);
    time.setFillColor(secs <= 10 ? sf::Color(255, 90, 90) : sf::Color::White);
    const sf::FloatRect tb = time.getLocalBounds();
    time.setOrigin(tb.left + tb.width / 2.f, 0.f);
    time.setPosition(WINDOW_WIDTH / 2.f, 8.f);
    window_.draw(time);

    sf::Text found("Found: " + std::to_string(requiredFound_) + "/" + std::to_string(requiredTotal_), font_, 24);
    const sf::FloatRect fb = found.getLocalBounds();
    found.setOrigin(fb.left + fb.width, 0.f);
    found.setPosition(WINDOW_WIDTH - 16.f, 8.f);
    window_.draw(found);

    if (messageTimer_ > 0.f && !message_.empty()) {
        drawCenteredText(message_, 24, 60.f, sf::Color(255, 240, 120));
    }
}

void Game::renderPaused() {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window_.draw(overlay);
    drawCenteredText("PAUSED", 64, 280.f, sf::Color::White);
    drawCenteredText("P / ESC: resume     M: main menu", 24, 370.f, sf::Color(200, 210, 230));
}

void Game::renderResults() {
    drawCenteredText(endReason_, 56, 150.f, sf::Color(255, 200, 40));
    drawCenteredText("Score: " + std::to_string(scoreManager_.getScore()), 40, 260.f, sf::Color::White);
    drawCenteredText("Items found: " + std::to_string(requiredFound_) + " / " + std::to_string(requiredTotal_),
                     30, 330.f, sf::Color::White);
    drawCenteredText("Best score: " + std::to_string(scoreManager_.getBestScore()), 30, 385.f,
                     sf::Color(120, 220, 140));
    if (scoreManager_.isNewBest()) {
        drawCenteredText("NEW BEST SCORE!", 30, 440.f, sf::Color(255, 120, 200));
    }
    drawCenteredText("ENTER: play again     M: main menu", 26, 560.f, sf::Color(200, 210, 230));
}
