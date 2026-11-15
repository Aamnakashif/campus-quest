// ScoreManager.h
// Keeps the current score and the best score, and saves the best score to a text file.
// Demonstrates: encapsulation, file handling and exception handling.
#pragma once
#include <stdexcept>
#include <string>

// Custom exception used for problems with the best-score file.
class ScoreFileException : public std::runtime_error {
public:
    explicit ScoreFileException(const std::string& message) : std::runtime_error(message) {}
};

class ScoreManager {
public:
    explicit ScoreManager(std::string filePath = "best_score.txt");

    void resetScore();                 // start of a new round
    void addPoints(int points);        // throws std::invalid_argument if points < 0

    // Reads the best score from the file. NEVER throws: a missing file means 0,
    // and a broken file is reported through getLastError() and treated as 0.
    void loadBestScore();

    // If the current score beats the best, stores and saves it. Returns true on a new best.
    // A file write problem is reported through getLastError(); the game keeps running.
    bool saveBestIfNeeded();

    int getScore() const { return score_; }
    int getBestScore() const { return bestScore_; }
    bool isNewBest() const { return newBest_; }
    const std::string& getLastError() const { return lastError_; }

private:
    int readBestFromFile() const;   // throws ScoreFileException
    void writeBestToFile() const;   // throws ScoreFileException

    std::string filePath_;
    int score_;
    int bestScore_;
    bool newBest_;
    std::string lastError_;
};
