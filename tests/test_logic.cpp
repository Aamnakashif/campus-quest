// test_logic.cpp - simple unit tests (no test framework needed, no window needed)
#include <cstdio>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
#include "CampusMap.h"
#include "LevelData.h"
#include "Player.h"
#include "PointItem.h"
#include "ScoreManager.h"
#include "TimeBonus.h"

static int g_failed = 0;
static int g_total = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        ++g_total;                                                               \
        if (!(cond)) {                                                           \
            ++g_failed;                                                          \
            std::cout << "  FAIL  line " << __LINE__ << ": " #cond << std::endl; \
        }                                                                        \
    } while (0)

#define CHECK_THROWS(expr, ExType)                                               \
    do {                                                                         \
        ++g_total;                                                               \
        bool threw = false;                                                      \
        try { expr; } catch (const ExType&) { threw = true; }                    \
        if (!threw) {                                                            \
            ++g_failed;                                                          \
            std::cout << "  FAIL  line " << __LINE__ << ": expected " #ExType << std::endl; \
        }                                                                        \
    } while (0)

// A fake game used so collectibles can be tested without opening a window.
class MockContext : public IGameContext {
public:
    int score = 0;
    float time = 0.f;
    std::string lastMessage;
    void addScore(int p) override { score += p; }
    void addTime(float s) override { time += s; }
    void showMessage(const std::string& t) override { lastMessage = t; }
};

static void writeFile(const std::string& path, const std::string& text) {
    std::ofstream out(path, std::ios::trunc);
    out << text;
}

static void testCollectibles() {
    std::cout << "Collectibles\n";
    MockContext ctx;
    PointItem book("Book", sf::Vector2f(100.f, 100.f), 10);
    TimeBonus watch("Watch", sf::Vector2f(200.f, 200.f), 7.f);

    CHECK(!book.isCollected());
    CHECK(book.isRequired());
    CHECK(!watch.isRequired());

    CHECK(book.collect(ctx));         // first time works
    CHECK(book.isCollected());
    CHECK(ctx.score == 10);
    CHECK(!book.collect(ctx));        // second time does nothing
    CHECK(ctx.score == 10);

    CHECK(watch.collect(ctx));
    CHECK(ctx.time == 7.f);
    CHECK(!watch.collect(ctx));
    CHECK(ctx.time == 7.f);
    CHECK(ctx.score == 10);           // time bonus never changes score

    // polymorphism through the base class pointer
    std::unique_ptr<Collectible> base = std::make_unique<PointItem>("Cup", sf::Vector2f(0.f, 0.f), 5);
    base->collect(ctx);
    CHECK(ctx.score == 15);

    CHECK_THROWS(PointItem("Bad", sf::Vector2f(0.f, 0.f), 0), std::invalid_argument);
    CHECK_THROWS(PointItem("Bad", sf::Vector2f(0.f, 0.f), -3), std::invalid_argument);
    CHECK_THROWS(TimeBonus("Bad", sf::Vector2f(0.f, 0.f), 0.f), std::invalid_argument);
}

static void testScoreManager() {
    std::cout << "ScoreManager\n";
    const std::string path = "test_best_score.tmp";
    std::remove(path.c_str());

    ScoreManager sm(path);
    sm.loadBestScore();                       // file missing -> 0, no error
    CHECK(sm.getBestScore() == 0);
    CHECK(sm.getLastError().empty());

    sm.addPoints(10);
    sm.addPoints(5);
    CHECK(sm.getScore() == 15);
    CHECK_THROWS(sm.addPoints(-1), std::invalid_argument);

    CHECK(sm.saveBestIfNeeded());             // 15 > 0 -> new best
    CHECK(sm.isNewBest());
    CHECK(sm.getBestScore() == 15);

    ScoreManager again(path);                 // best score survives "restart"
    again.loadBestScore();
    CHECK(again.getBestScore() == 15);

    again.resetScore();
    again.addPoints(10);
    CHECK(!again.saveBestIfNeeded());         // 10 < 15 -> not a new best
    CHECK(again.getBestScore() == 15);

    writeFile(path, "not a number");          // damaged file must not crash the game
    ScoreManager broken(path);
    broken.loadBestScore();
    CHECK(broken.getBestScore() == 0);
    CHECK(!broken.getLastError().empty());

    writeFile(path, "-5");                    // negative is also invalid
    ScoreManager negative(path);
    negative.loadBestScore();
    CHECK(negative.getBestScore() == 0);
    CHECK(!negative.getLastError().empty());

    ScoreManager unwritable("/nonexistent_folder_xyz/best.txt");   // cannot write -> no crash
    unwritable.addPoints(5);
    CHECK(unwritable.saveBestIfNeeded());     // kept in memory
    CHECK(unwritable.getBestScore() == 5);
    CHECK(!unwritable.getLastError().empty());

    std::remove(path.c_str());
}

static void testMapAndPlayer() {
    std::cout << "CampusMap and Player\n";
    CampusMap map(sf::FloatRect(0.f, 0.f, 200.f, 200.f));
    map.addObstacle(sf::FloatRect(100.f, 0.f, 20.f, 200.f));   // wall in the middle

    CHECK(map.isWalkable(sf::FloatRect(10.f, 10.f, 20.f, 20.f)));
    CHECK(!map.isWalkable(sf::FloatRect(-5.f, 10.f, 20.f, 20.f)));    // outside left
    CHECK(!map.isWalkable(sf::FloatRect(190.f, 10.f, 20.f, 20.f)));   // outside right
    CHECK(!map.isWalkable(sf::FloatRect(95.f, 10.f, 20.f, 20.f)));    // touches wall

    Player p(sf::Vector2f(50.f, 50.f), 100.f, 20.f);
    p.move(sf::Vector2f(1.f, 0.f), 0.1f, map);                         // 10 px right
    CHECK(p.getPosition().x > 59.9f && p.getPosition().x < 60.1f);

    for (int i = 0; i < 200; ++i) p.move(sf::Vector2f(1.f, 0.f), 0.05f, map);   // walk into wall
    CHECK(p.getBounds().left + p.getBounds().width <= 100.f);          // stopped at wall

    float yBefore = p.getPosition().y;
    p.move(sf::Vector2f(1.f, 1.f), 0.1f, map);                         // slides down along wall
    CHECK(p.getPosition().y > yBefore);

    for (int i = 0; i < 500; ++i) p.move(sf::Vector2f(-1.f, -1.f), 0.05f, map); // map edges
    CHECK(map.isWalkable(p.getBounds()));

    CHECK_THROWS(Player(sf::Vector2f(0.f, 0.f), 0.f), std::invalid_argument);
    CHECK_THROWS(p.setSpeed(-1.f), std::invalid_argument);
}

static void testDefaultLevel() {
    std::cout << "Default level data\n";
    CampusMap map = CampusMap::createDefaultCampus();
    CHECK(map.getLandmarks().size() == 6);

    Player player(LevelData::PLAYER_START);
    CHECK(map.isWalkable(player.getBounds()));            // start is on open ground

    auto items = LevelData::makeDefaultCollectibles();
    CHECK(items.size() == 10);
    int required = 0;
    std::set<std::pair<int,int>> goals;
    for (const auto& item : items) {
        CHECK(map.isWalkable(item->getBounds()));         // no item is hidden inside a wall
        if (item->isRequired()) ++required;
    }
    CHECK(required == 8);

    // Flood-fill on a 4 px grid: every item must be REACHABLE from the start.
    const float cell = 4.f;
    const int W = static_cast<int>(map.getBounds().width / cell);
    const int H = static_cast<int>((map.getBounds().top + map.getBounds().height) / cell);
    auto walkable = [&](int cx, int cy) {
        Player probe(sf::Vector2f(cx * cell, cy * cell));
        return map.isWalkable(probe.getBounds());
    };
    std::vector<std::vector<bool>> seen(H + 1, std::vector<bool>(W + 1, false));
    std::queue<std::pair<int,int>> q;
    const int sx = static_cast<int>(LevelData::PLAYER_START.x / cell);
    const int sy = static_cast<int>(LevelData::PLAYER_START.y / cell);
    q.push({sx, sy});
    seen[sy][sx] = true;
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        const int dx[4] = {1, -1, 0, 0};
        const int dy[4] = {0, 0, 1, -1};
        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k], ny = y + dy[k];
            if (nx < 0 || ny < 0 || nx > W || ny > H || seen[ny][nx]) continue;
            if (!walkable(nx, ny)) continue;
            seen[ny][nx] = true;
            q.push({nx, ny});
        }
    }
    for (const auto& item : items) {
        const int ix = static_cast<int>(item->getPosition().x / cell);
        const int iy = static_cast<int>(item->getPosition().y / cell);
        CHECK(seen[iy][ix]);
    }
}

int main() {
    testCollectibles();
    testScoreManager();
    testMapAndPlayer();
    testDefaultLevel();
    std::cout << "\n" << (g_total - g_failed) << " / " << g_total << " checks passed\n";
    return g_failed == 0 ? 0 : 1;
}
