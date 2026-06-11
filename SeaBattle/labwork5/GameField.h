#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <vector>
#include <string>
#include "Ship.h"

class GameField {
private:
    int width, height;
    std::vector<std::vector<char>> field;
    std::vector<Ship> ships;

public:
    GameField(int w, int h);
    bool placeShip(int size, char orientation, int x, int y);
    std::string shoot(int x, int y);
    void saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);
    bool allShipsDestroyed() const;
    int getWidth() const;
    int getHeight() const;
};

#endif 
