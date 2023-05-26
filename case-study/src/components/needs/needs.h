#ifndef INCLUDED_NEEDS_H
#define INCLUDED_NEEDS_H

#include "../../flecs/flecs.h"
#include "../traits/traits.h"
#include "../components/components.h"
#include "../environment/environment.h"

typedef struct Need
{
    double level;
} Need;

typedef struct Energy : public Need
{} Energy;

typedef struct Productivity : public Need
{} Productivity;

typedef struct Fun : public Need
{} Fun;

typedef struct Hunger : public Need
{} Hunger;

double getNeedUpdate(double current, double update);
bool doReplaceAction(flecs::entity *entity, double newActionProbability);

// Functions for Needs Systems
void checkEnergy(flecs::entity e, Active const &active, Energy &energy, Inventory const &inventory);
void checkStudentProductivity(flecs::entity e, Active const &active, Productivity &productivity, Driven const &driven);
void checkProductivity(flecs::entity e, Active const &active, Productivity &productivity, Workaholic const &workaholic);
void checkFun(flecs::entity e, Active const &active, Fun &fun);
void checkHunger(flecs::entity e, Active const &active, Hunger &hunger, Inventory const &inventory);

#endif