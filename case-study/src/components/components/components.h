#ifndef INCLUDED_COMPONENTS_H
#define INCLUDED_COMPONENTS_H

#include <vector>
#include <string>
#include <iosfwd>

typedef struct Object
{
    std::string name;
} Object;

typedef struct Food : public Object
{
    double level;
} Food;

typedef struct Active
{
} Active;

typedef struct Inactive
{
} Inactive;

typedef struct Clock
{
    double actualTime;
    long stepTime;
    double stepLength;
} Clock;

typedef struct Inventory
{
    std::vector<Food> food;
    std::vector<Food> coffee;
} Inventory;

typedef struct Position
{
    Position() : x(0), y(0) {}
    Position(Position const &p) : x(p.x), y(p.y) {}
    Position(int x_, int y_) : x(x_), y(y_) {}
    int x, y;
} Position;

bool operator!(Position const &pos);
bool operator==(Position const &lhs, Position const &rhs);
std::ostream &operator<<(std::ostream &out, Position const &pos);

bool operator==(Food const &lhs, Food const &rhs);

#endif