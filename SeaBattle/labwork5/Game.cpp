#include "Game.h"
#include <sstream>
#include <iostream>
#include <fstream>

Game::Game() : field(nullptr), role(""), gameStarted(false), strategy("ordered"), shootIndex(0), lastShotCoords(-1, -1), currentDirection(0) {
    directions = {{-1, 0}, {0, 1}, {0, -1}, {1, 0}}; 
}

std::pair<int, int> Game::getOrderedShoot() {
    int x = shootIndex % field->getWidth();
    int y = shootIndex / field->getWidth();
    shootIndex++;
    return {x, y};
}

std::pair<int, int> Game::getCustomShoot() {
    if (lastShotResult == "hit") {
        for (size_t i = currentDirection; i < directions.size(); ++i) {
            int dx = directions[i].first;
            int dy = directions[i].second;
            int nx = lastShotCoords.first + dx;
            int ny = lastShotCoords.second + dy;
            if (nx >= 0 && nx < field->getWidth() && ny >= 0 && ny < field->getHeight()) {
                return {nx, ny};
            }
        }
    }
    return getOrderedShoot();
}

std::string Game::setResult(const std::string& result) {
    if (result == "hit" || result == "miss" || result == "kill") {
        lastShotResult = result;
        if (result == "hit") {
            currentDirection = 0;  
        }
        return "ok";
    }
    return "error";
}

std::string Game::createGame(const std::string& roleType) {
    if (gameStarted) return "error: game already started";
    role = roleType;
    field = new GameField(10, 10); 
    return "ok";
}

std::string Game::setFieldSize(int w, int h) {
    if (gameStarted) return "error: game already started";
    if (field != nullptr) {
        delete field;
    }
    field = new GameField(w, h);
    return "ok";
}

std::string Game::setShipCount(int type, int count) {
    if (gameStarted) return "error: game already started";
    if (type < 1 || type > 4 || count < 1) return "error: invalid ship type or count";
    shipCounts[type] = count;
    return "ok";
}

std::string Game::startGame() {
    if (gameStarted) return "error: game already started";
    if (role.empty()) return "error: role not set";
    gameStarted = true;
    return "ok";
}

std::string Game::stopGame() {
    if (!gameStarted) return "error: game not started";
    gameStarted = false;
    delete field;
    field = nullptr;
    return "ok";
}

std::string Game::dumpField(const std::string& path) const {
    if (!field) return "error: no field initialized";
    field->saveToFile(path);
    return "ok";
}

std::string Game::loadField(const std::string& path) {
    if (!field) return "error: no field initialized";
    if (field->loadFromFile(path)) {
        return "ok";
    }
    return "error: failed to load field";
}

std::string Game::setStrategy(const std::string& newStrategy) {
    if (newStrategy == "ordered" || newStrategy == "custom") {
        strategy = newStrategy;
        return "ok";
    }
    return "error: invalid strategy";
}

std::string Game::shoot(int x, int y) {
    if (!gameStarted) return "error: game not started";
    if (x < 0 || x >= field->getWidth() || y < 0 || y >= field->getHeight()) return "error: invalid coordinates";
    std::string result = field->shoot(x, y);
    lastShotCoords = {x, y};
    return result;
}

std::pair<int, int> Game::getShootCoordinates() {
    if (strategy == "ordered") {
        return getOrderedShoot();
    } else if (strategy == "custom") {
        return getCustomShoot();
    }
    return {0, 0}; 
}


int Game::getWidth() const {
    return field->getWidth();
}

int Game::getHeight() const {
    return field->getHeight();
}

int Game::getShipCount(int type) const {
    auto it = shipCounts.find(type);
    if (it != shipCounts.end()) {
        return it->second;
    }
    return 0;
}

bool Game::isFinished() const {
    return field->allShipsDestroyed();
}

bool Game::isWin() const {
    return isFinished() && role == "defender";
}

bool Game::isLose() const {
    return isFinished() && role == "attacker";
}

std::string Game::getStatus() {
    if (isWin()) return "win";
    if (isLose()) return "lose";
    return "in progress";
}

Game::~Game() {
    if (field) {
        delete field;
    }
}
