#include "GameField.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

GameField::GameField(int w, int h) : width(w), height(h), field(h, std::vector<char>(w, '.')) {}

bool GameField::placeShip(int size, char orientation, int x, int y) {
    if (orientation == 'h') {
        if (x + size > width) return false;
        for (int i = 0; i < size; ++i) {
            if (field[y][x + i] != '.') return false;
        }
        for (int i = 0; i < size; ++i) {
            field[y][x + i] = 'S';
        }
    } else if (orientation == 'v') {
        if (y + size > height) return false;
        for (int i = 0; i < size; ++i) {
            if (field[y + i][x] != '.') return false;
        }
        for (int i = 0; i < size; ++i) {
            field[y + i][x] = 'S';
        }
    }

    ships.push_back({size, orientation, x, y});
    return true;
}

std::string GameField::shoot(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return "miss";
    if (field[y][x] == 'S') {
        field[y][x] = 'X';
        ships.erase(std::remove_if(ships.begin(), ships.end(), [x, y](const Ship& ship) {
            if (ship.orientation == 'h') {
                return (y == ship.y && x >= ship.x && x < ship.x + ship.size);
            } else {
                return (x == ship.x && y >= ship.y && y < ship.y + ship.size);
            }
        }), ships.end());

        if (ships.empty()) return "kill";
        return "hit";
    }
    return "miss";
}

void GameField::saveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file) {
        std::cerr << "Error opening file for saving: " << path << std::endl;
        return;
    }
    file << width << " " << height << std::endl;
    for (const auto& ship : ships) {
        file << ship.size << " " << ship.orientation << " " << ship.x << " " << ship.y << std::endl;
    }
    file.close();
}

bool GameField::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Error opening file for loading: " << path << std::endl;
        return false;
    }
    file >> width >> height;
    if (file.fail() || width <= 0 || height <= 0) {
        std::cerr << "Invalid data in file: " << path << std::endl;
        return false;
    }

    field = std::vector<std::vector<char>>(height, std::vector<char>(width, '.'));
    ships.clear();

    int size;
    char orientation;
    int x, y;
    while (file >> size >> orientation >> x >> y) {
        if (!placeShip(size, orientation, x, y)) {
            std::cerr << "Failed to place ship at (" << x << ", " << y << ")" << std::endl;
            return false;
        }
    }
    file.close();
    return true;
}

bool GameField::allShipsDestroyed() const {
    return ships.empty();
}

int GameField::getWidth() const {
    return width;
}

int GameField::getHeight() const {
    return height;
}
