// ScoreManager.cpp
#include "ScoreManager.h"
#include <fstream>
#include <stdexcept>
#include <utility>

ScoreManager::ScoreManager(std::string filePath)
    : filePath_(std::move(filePath)), score_(0), bestScore_(0), newBest_(false) {}

void ScoreManager::resetScore() {
    score_ = 0;
    newBest_ = false;
}

void ScoreManager::addPoints(int points) {
    if (points < 0) {
        throw std::invalid_argument("ScoreManager: points cannot be negative");
    }
    score_ += points;
}

int ScoreManager::readBestFromFile() const {
    std::ifstream in(filePath_);
    if (!in) {
        return 0;   // no file yet (first run) is normal, not an error
    }
    int value = 0;
    if (!(in >> value) || value < 0) {
        throw ScoreFileException("Best-score file is damaged: " + filePath_);
    }
    return value;
}

void ScoreManager::writeBestToFile() const {
    std::ofstream out(filePath_, std::ios::trunc);
    if (!out) {
        throw ScoreFileException("Cannot write best-score file: " + filePath_);
    }
    out << bestScore_ << '\n';
    if (!out) {
        throw ScoreFileException("Error while writing best-score file: " + filePath_);
    }
}

void ScoreManager::loadBestScore() {
    lastError_.clear();
    try {
        bestScore_ = readBestFromFile();
    } catch (const ScoreFileException& e) {
        bestScore_ = 0;           // keep the game running with a valid value
        lastError_ = e.what();
    }
}

bool ScoreManager::saveBestIfNeeded() {
    if (score_ <= bestScore_) {
        return false;
    }
    bestScore_ = score_;
    newBest_ = true;
    try {
        writeBestToFile();
    } catch (const ScoreFileException& e) {
        lastError_ = e.what();    // best score still kept in memory for this session
    }
    return true;
}
