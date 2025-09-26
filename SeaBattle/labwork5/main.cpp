#include <iostream>
#include <sstream>
#include "Game.h"

int main() {
    setlocale(LC_ALL, "Russian");

    Game game;
    std::string command;

    while (std::getline(std::cin, command)) {
        std::istringstream iss(command);
        std::string action;
        iss >> action;

        std::string result = "ok";

        if (action == "ping") {
            result = "pong";
        } else if (action == "exit") {
            result = "ok";
            break;
        } else if (action == "create") {
            std::string role;
            iss >> role;
            result = game.createGame(role);
        } else if (action == "start") {
            result = game.startGame();
        } else if (action == "set") {
            std::string param;
            iss >> param;

            if (param == "width") {
                int width;
                iss >> width;
                result = game.setFieldSize(width, game.getHeight()); 
            } else if (param == "height") {
                int height;
                iss >> height;
                result = game.setFieldSize(game.getWidth(), height);  
            } else if (param == "count") {
                int type, count;
                iss >> type >> count;
                result = game.setShipCount(type, count);
            } else if (param == "strategy") {
                std::string strategy;
                iss >> strategy;
                result = game.setStrategy(strategy);
            } else {
                result = "unknown command";
            }
        } else if (action == "get shot") {
            auto coords = game.getShootCoordinates();
            result = std::to_string(coords.first) + " " + std::to_string(coords.second);
        } else if (action == "shot") {
            int x, y;
            iss >> x >> y;
            result = game.shoot(x, y);
        } else if (action == "set result") {
            std::string res;
            iss >> res;
            result = game.setResult(res);
        } else if (action == "stop") {
            result = game.stopGame();
        } else if (action == "dump") {
            std::string path;
            iss >> path;
            result = game.dumpField(path);
        } else if (action == "load") {
            std::string path;
            iss >> path;
            result = game.loadField(path);
        } else if (action == "finished") {
            result = (game.isFinished() ? "yes" : "no");
        } else if (action == "win") {
            result = (game.isWin() ? "yes" : "no");
        } else if (action == "lose") {
            result = (game.isLose() ? "yes" : "no");
        } else if (action == "get") {
            std::string param;
            iss >> param;
            if (param == "width") {
                result = std::to_string(game.getWidth());
            } else if (param == "height") {
                result = std::to_string(game.getHeight());
            } else if (param == "count") {
                int type;
                iss >> type;
                result = std::to_string(game.getShipCount(type));
            } else {
                result = "unknown command";
            }
        } else if (action == "shoot-coordinates") {
            auto coords = game.getShootCoordinates();
            result = std::to_string(coords.first) + " " + std::to_string(coords.second);
        } else {
            result = "unknown command";
        }

        std::cout << result << std::endl;
    }

    return 0;
}
