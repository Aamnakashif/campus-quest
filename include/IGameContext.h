// IGameContext.h
// Abstract interface that a Collectible is allowed to talk to.
// Game implements it. Using an interface (abstraction) means collectibles
// do not depend on the whole Game class, and they can be tested without a window.
#pragma once
#include <string>

class IGameContext {
public:
    virtual ~IGameContext() = default;

    virtual void addScore(int points) = 0;                  // give the player points
    virtual void addTime(float seconds) = 0;                // extend the round timer
    virtual void showMessage(const std::string& text) = 0;  // short on-screen message
};
