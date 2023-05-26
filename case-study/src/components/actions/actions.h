#ifndef INCLUDED_ACTIONS_H
#define INCLUDED_ACTIONS_H

#include <vector>

#include "../../flecs/flecs.h"

#include "../traits/traits.h"
#include "../needs/needs.h"
#include "../components/components.h"
#include "../environment/environment.h"
#include "../components/trace/trace.h"
#include "../calendar/calendar.h"

typedef enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
} Direction;

typedef enum ActionEnum
{
    WALKING,
    DRINKING_COFFEE,
    EATING,
    PLAYING_GAME,
    ATTENDING_LECTURE,
    STUDYING,
    GIVING_LECTURE,
    WORKING
} ActionEnum;

typedef struct CurrentAction
{
    ActionEnum action;
    double probablity;
} CurrentAction;

typedef struct Path
{
    std::vector<Destination> path;
} Path;

typedef struct Idle
{
} Idle;

typedef struct Walking
{
    Walking() : speedInSteps(15), direction(NONE), path() {}
    size_t speedInSteps;
    Direction direction;
    Path path;
} Walking;

typedef struct DrinkingCoffee
{
    DrinkingCoffee() {}
    DrinkingCoffee(Food c) : coffee(c) {}
    Food coffee;
} Drinking;

typedef struct Eating
{
    Eating() {}
    Eating(Food f) : food(f) {}
    Food food;
} Eating;

typedef struct PlayingGame
{

} PlayingGame;

typedef struct GivingLecture
{
    std::string lecName;
    std::string location;
    double startTime;
    double endTime;
} GivingLecture;

typedef struct AttendingLecture
{
    std::string lecName;
    std::string location;
    double startTime;
    double endTime;
} AttendingLecture;

typedef struct Studying
{
    std::string workspaceName;
} Studying;

typedef struct Working
{
    std::string workspaceName;
} Working;

void removeAction(flecs::entity *entity, ActionEnum action);
void removeAllActions(flecs::entity *entity);

// Functions for Action Systems
void doIdle(flecs::entity e, Trace &trace, Idle const &idle);
void doWalk(flecs::entity e, Trace &trace, Walking &walking, Destination const &des, Position &pos, Energy &energy);
void doDrink(flecs::entity e, Trace &trace, DrinkingCoffee &drinking, Energy &energy);
void doPlayGame(flecs::entity e, Trace &trace, PlayingGame const &playing, Fun &fun);
void doEat(flecs::entity e, Trace &trace, Eating &eating, Hunger &hunger, Energy &energy);

void doStudy(flecs::entity e, Trace &trace, Studying const &studying, Energy &energy, Productivity &productivity, Fun &fun);
void doWork(flecs::entity e, Trace &trace, Working const &working, Energy &energy, Productivity &productivity, Fun &fun);
void doGiveLecture(flecs::entity e, Trace &trace, GivingLecture const &lecture, Energy &energy, Productivity &productivity, Fun &fun);
void doAttendLecture(flecs::entity e, Trace &trace, AttendingLecture const &lecture, Energy &energy, Productivity &productivity, Fun &fun);

#endif