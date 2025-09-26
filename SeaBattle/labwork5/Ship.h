#ifndef SHIP_H
#define SHIP_H

class Ship {
public:
    int size;
    char orientation;
    int x, y;

    Ship(int size = 0, char orientation = 'h', int x = 0, int y = 0)
        : size(size), orientation(orientation), x(x), y(y) {}
};

#endif 
