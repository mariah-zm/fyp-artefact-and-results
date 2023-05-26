#include "needs.ih"

void checkEnergy(flecs::entity e, Active const &active, Energy &energy, Inventory const &inventory)
{
    // cout << "Energy System\n";

    // Update Energy level in simulation step
    energy.level = getNeedUpdate(energy.level, DEC_ENERGY);

    auto rulesEngine = e.world().get<RulesEngine>()->coffeeEngine;
    rulesEngine->setInputValue(ENERGY_INPUT, energy.level);
    rulesEngine->process();

    auto outputCoffee = rulesEngine->getOutputVariable(COFFEE_OUTPUT);
    outputCoffee->defuzzify();
    double coffeeProbability = outputCoffee->getValue();

    if (coffeeProbability > 0.5 && !e.has<DrinkingCoffee>() && doReplaceAction(&e, coffeeProbability) && !inventory.coffee.empty())
    {
        e.set<DrinkingCoffee>(inventory.coffee.at(0));
        e.set<CurrentAction>({DRINKING_COFFEE, coffeeProbability});
    }
}

void checkStudentProductivity(flecs::entity e, Active const &active, Productivity &productivity, Driven const &driven)
{
    // Update Productivity level in simulation step and add decrease effect of Driven level
    productivity.level = getNeedUpdate(productivity.level, DEC_PRODUCTIVIY * (1 + driven.level));

    auto rulesEngine = e.world().get<RulesEngine>()->workEngine;
    rulesEngine->setInputValue(ENERGY_INPUT, e.get<Energy>()->level);
    rulesEngine->setInputValue(PRODUCTIVITY_INPUT, productivity.level);
    rulesEngine->setInputValue(DRIVEN_INPUT, driven.level);
    rulesEngine->process();

    auto outputStudy = rulesEngine->getOutputVariable(STUDY_WORK_OUTPUT);
    outputStudy->defuzzify();
    double studyProbability = outputStudy->getValue();

    if (studyProbability > 0.5 && !e.has<Studying>() && doReplaceAction(&e, studyProbability))
    {
        // Set random workspace in library
        auto workspaces = e.world().lookup("LIBRARY").get<Building>()->objects;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, workspaces.size() - 1);
        size_t randIdx = dis(gen);

        string workspaceName = workspaces.at(randIdx).name;

        e.set<Studying>({workspaceName});
        e.set<Destination>({workspaceName});
        e.set<CurrentAction>({STUDYING, studyProbability});
    }
}

void checkProductivity(flecs::entity e, Active const &active, Productivity &productivity, Workaholic const &workaholic)
{
    // Update Productivity level in simulation step and add decrease effect of Workaholic level
    productivity.level = getNeedUpdate(productivity.level, DEC_PRODUCTIVIY * (1 + workaholic.level));

    auto rulesEngine = e.world().get<RulesEngine>()->workEngine;
    rulesEngine->setInputValue(ENERGY_INPUT, e.get<Energy>()->level);
    rulesEngine->setInputValue(PRODUCTIVITY_INPUT, productivity.level);
    rulesEngine->setInputValue(DRIVEN_INPUT, workaholic.level);
    rulesEngine->process();

    auto outputWork = rulesEngine->getOutputVariable(STUDY_WORK_OUTPUT);
    outputWork->defuzzify();
    double workProbability = outputWork->getValue();

    if (workProbability > 0.5 && !e.has<Working>() && doReplaceAction(&e, workProbability))
    {
        string assignedWorkspace = e.has<Admin>() ? e.get<Admin>()->assignedWorkspace : e.get<Professor>()->assignedWorkspace;

        e.set<Working>({assignedWorkspace});
        e.set<Destination>({assignedWorkspace});
        e.set<CurrentAction>({WORKING, workProbability});
    }
}

void checkFun(flecs::entity e, Active const &active, Fun &fun)
{
    // Update Fun level in simulation step
    fun.level = getNeedUpdate(fun.level, DEC_ENERGY);

    double drivenLevel = e.has<Driven>() ? e.get<Driven>()->level : e.get<Workaholic>()->level;

    auto rulesEngine = e.world().get<RulesEngine>()->playEngine;
    rulesEngine->setInputValue(FUN_INPUT, fun.level);
    rulesEngine->setInputValue(DRIVEN_INPUT, drivenLevel);
    rulesEngine->process();

    auto outputPlay = rulesEngine->getOutputVariable(PLAY_OUTPUT);
    outputPlay->defuzzify();
    double playProbability = outputPlay->getValue();

    if (playProbability > 0.7 && !e.has<PlayingGame>() && doReplaceAction(&e, playProbability))
    {
        e.add<PlayingGame>();
        e.set<Destination>({"QUADRANGLE"});
        e.set<CurrentAction>({PLAYING_GAME, playProbability});
    }
}

void checkHunger(flecs::entity e, Active const &active, Hunger &hunger, Inventory const &inventory)
{
    hunger.level = getNeedUpdate(hunger.level, DEC_HUNGER);

    auto rulesEngine = e.world().get<RulesEngine>()->eatEngine;
    rulesEngine->setInputValue(ENERGY_INPUT, e.get<Energy>()->level);
    rulesEngine->setInputValue(HUNGER_INPUT, hunger.level);
    rulesEngine->process();

    auto outputEat = rulesEngine->getOutputVariable(EAT_OUTPUT);
    outputEat->defuzzify();
    double eatProbability = outputEat->getValue();

    if (eatProbability > 0.5 && !e.has<Eating>() && doReplaceAction(&e, eatProbability) && !inventory.food.empty())
    {
        e.set<Eating>(inventory.food.at(0));
        e.set<Destination>({"CANTEEN"});
        e.set<CurrentAction>({EATING, eatProbability});
    }
}

double getNeedUpdate(double currentValue, double change)
{
    double newValue = currentValue + currentValue * change;

    if (newValue < 0)
        return 0;
    else if (newValue > 1)
        return 1;
    else
        return newValue;
}

bool doReplaceAction(flecs::entity *entity, double newActionProbability)
{
    if (entity->has<CurrentAction>())
    {
        auto current = entity->get<CurrentAction>();

        if (current->probablity < newActionProbability)
        {
            removeAllActions(entity);
            return true;
        }
        else
            return false;
    }
    else if (entity->has<Walking>() || entity->has<GivingLecture>() || entity->has<AttendingLecture>())
        return false;
    else
    {
        if (entity->has<Idle>())
            entity->remove<Idle>();

        return true;
    }
}
