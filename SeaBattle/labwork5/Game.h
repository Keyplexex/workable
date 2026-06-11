#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <random>
#include "GameField.h"

class Game {
private:
    GameField* field;
    std::string role;
    std::map<int, int> shipCounts;
    bool gameStarted;
    std::string strategy;
    int shootIndex;
    std::string lastShotResult;
    std::pair<int, int> lastShotCoords;
    size_t currentDirection;
    std::vector<std::pair<int, int>> directions;

    std::pair<int, int> getOrderedShoot();
    std::pair<int, int> getCustomShoot();

public:
    Game();
    std::string setResult(const std::string& result);
    std::pair<int, int> getShootCoordinates();
    std::string createGame(const std::string& roleType);
    std::string setFieldSize(int w, int h);
    std::string setShipCount(int type, int count);
    std::string startGame();
    std::string stopGame();
    std::string dumpField(const std::string& path) const;
    std::string loadField(const std::string& path);
    std::string setStrategy(const std::string& newStrategy);
    std::string shoot(int x, int y);
    int getWidth() const;
    int getHeight() const;
    int getShipCount(int type) const;
    bool isFinished() const;
    bool isWin() const;
    bool isLose() const;
    std::string getStatus();
    ~Game();
};

#endif 
