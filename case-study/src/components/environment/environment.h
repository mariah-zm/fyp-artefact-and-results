#ifndef INCLUDED_ENVIRONMENT_H
#define INCLUDED_ENVIRONMENT_H

#include <string>
#include <vector>
#include <map>

#include "../../flecs/flecs.h"
#include "../components/components.h"

typedef struct VendingMachine : public Object
{
    int width;
    int height;
    Position position;
    std::vector<Food> snacks;
    bool isInUse;
} VendingMachine;

typedef struct CoffeeStall : public Object
{
    int width;
    int height;
    Position position;
    bool isInUse;
} CoffeeStall;

typedef struct Area
{
    std::string name;
    size_t capacity;
    int width;
    int height;
    Position position;
    std::vector<Object> objects;
} Area;

typedef struct Room : public Area
{
    Position entrance;
    std::string building;
} Room;

typedef struct LectureHall : public Room
{
    bool hasLectureStarted;
    bool hasLectureEnded;
    int attendance;
} LectureHall;

typedef struct Building : public Area
{
    Position entrance;
    std::vector<Room> rooms;
} Building;

typedef struct Workspace : public Object
{    
    int width;
    int height;
    Position position;
    size_t capacity;
    size_t occupancy;
} Workspace;

typedef struct Environment
{
    size_t width;
    size_t height;
    bool **isWalkable;
} Environment;

typedef struct Destination
{
    Destination() {}
    Destination(std::string loc) : location(loc) {}
    std::string location;
    std::vector<Position> path;
} Destination;

typedef struct Location
{
    std::string location;
} Location;

Position getRandomLocationInArea(Area const &area);
bool isPositionInArea(Position const pos, Area const &area);

Position getRandomLocationInArea(Position const &pos, int width, int height);
bool isPositionInArea(Position const pos, Position const areaPos, int width, int height);

#endif