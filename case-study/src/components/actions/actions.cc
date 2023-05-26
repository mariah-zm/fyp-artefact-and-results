#include "actions.ih"

// Private functions
Position getUpdatedPosition(Direction dir, Position pos, int steps);

void doIdle(flecs::entity e, Trace &trace, Idle const &idle)
{
    // Log Action
    trace.logAction("Idle");
}

void doWalk(flecs::entity e, Trace &trace, Walking &walking, Destination const &des, Position &pos, Energy &energy)
{
    if (!des.path.empty())
    {
        // Update Energy according to effect of action and its speed
        energy.level = getNeedUpdate(energy.level, DEC_ENERGY_WALK);
        auto destination = des.path.front();

        int x = pos.x;
        int y = pos.y;

        // Setting direction by aligning y-axis first and x-axis second;
        if (destination.y < y)
            walking.direction = UP;
        else if (destination.y > y)
            walking.direction = DOWN;
        else if (destination.x < x)
            walking.direction = LEFT;
        else if (destination.x > x)
            walking.direction = RIGHT;

        if (destination.y == y && abs(destination.x - x) > 15)
            walking.speedInSteps = 15;

        if ((walking.direction == UP || walking.direction == DOWN) && abs(destination.y - y) < walking.speedInSteps)
            walking.speedInSteps = ceil(walking.speedInSteps / 2.0);
        else if ((walking.direction == RIGHT || walking.direction == LEFT) && abs(destination.x - x) < walking.speedInSteps)
            walking.speedInSteps = ceil(walking.speedInSteps / 2.0);

        pos = getUpdatedPosition(walking.direction, pos, walking.speedInSteps);

        trace.logAction("Walking", {des.location});
    }
}

void doDrink(flecs::entity e, Trace &trace, DrinkingCoffee &drinking, Energy &energy)
{
    // Must have retrieved coffee
    if (true)
    {
        // Log Action
        trace.logAction("DrinkingCoffee", vector<string>(1, to_string(drinking.coffee.level)));

        // Update needs according to effect of action
        energy.level = getNeedUpdate(energy.level, INC_ENERGY_COFFEE);

        // Reduce coffee level - drink faster if lower energy
        drinking.coffee.level -= drinking.coffee.level * 0.05 * (2 - energy.level);

        auto rulesEngine = e.world().get<RulesEngine>()->coffeeEngine;
        auto highEnergyMembership = rulesEngine->getInputVariable(ENERGY_INPUT)->getTerm("HIGH")->membership(energy.level);

        // Check conditions whether action can be performed further
        if (drinking.coffee.level < 0.05 || highEnergyMembership > 0.7)
            e.remove<DrinkingCoffee>();
    }
}

void doPlayGame(flecs::entity e, Trace &trace, PlayingGame const &playing, Fun &fun)
{
    if (e.has<Destination>())
    {
        // Update needs according to action
        fun.level = getNeedUpdate(fun.level, INC_FUN_PLAY);
        trace.logAction("PlayingGame");

        // Calculate if Played enough
        auto rulesEngine = e.world().get<RulesEngine>()->playEngine;
        auto highFunMembership = rulesEngine->getInputVariable(FUN_INPUT)->getTerm("HIGH")->membership(fun.level);

        // Check conditions whether action can be performed further
        // Stop PLaying if Fun levels have been reached
        if (highFunMembership > 0.4)
            removeAction(&e, PLAYING_GAME);
    }
}

void doEat(flecs::entity e, Trace &trace, Eating &eating, Hunger &hunger, Energy &energy)
{
    // Check if food have been retrieved
    if (!e.has<Destination>())
    {
        // Log Action
        vector<string> components = {eating.food.name, to_string(eating.food.level)};
        trace.logAction("Eating", components);

        // Update needs according to action
        hunger.level = getNeedUpdate(hunger.level, INC_HUNGER_EAT);
        energy.level = getNeedUpdate(energy.level, INC_ENERGY_EAT);

        // Reduce food level - eat faster if lower hunger
        eating.food.level -= eating.food.level * 0.05 * (2 - hunger.level);

        auto rulesEngine = e.world().get<RulesEngine>()->eatEngine;
        auto highHungerMembership = rulesEngine->getInputVariable(HUNGER_INPUT)->getTerm("HIGH")->membership(hunger.level);

        // Check conditions whether action can be performed further
        // Stop Eating if food has finished or hunger levels have been satisfied
        if (eating.food.level < 0.05 || highHungerMembership > 0.8)
            removeAction(&e, EATING);
    }
}

void doStudy(flecs::entity e, Trace &trace, Studying const &studying, Energy &energy, Productivity &productivity, Fun &fun)
{
    auto workspace = e.world().lookup(studying.workspaceName.c_str()).get<Workspace>();

    // Must be at workspace to work - if have destination not yet arrived
    if (!e.has<Destination>() && isPositionInArea(*e.get<Position>(), workspace->position, workspace->width, workspace->height))
    {
        // Log Action
        trace.logAction("Studying", vector<string>(1, workspace->name));

        // Update needs according to action
        energy.level = getNeedUpdate(energy.level, DEC_ENERGY_STUDY);
        productivity.level = getNeedUpdate(productivity.level, INC_PRODUCTIVITY_STUDY);
        fun.level = getNeedUpdate(fun.level, DEC_FUN_STUDY);

        // Calculate if Productive enough
        auto rulesEngine = e.world().get<RulesEngine>()->workEngine;
        auto highProductivityMembership = rulesEngine->getInputVariable(PRODUCTIVITY_INPUT)->getTerm("HIGH")->membership(productivity.level);

        // Check conditions whether action can be performed further
        // Stop Studying if Productivity levels have been reached
        if (highProductivityMembership > 0.95)
            removeAction(&e, STUDYING);
    }
}

void doWork(flecs::entity e, Trace &trace, Working const &working, Energy &energy, Productivity &productivity, Fun &fun)
{
    auto workspace = e.world().lookup(working.workspaceName.c_str()).get<Workspace>();

    // Must be at workspace to work - if have destination not yet arrived
    if (!e.has<Destination>() && isPositionInArea(*e.get<Position>(), workspace->position, workspace->width, workspace->height))
    {
        // Log Action
        trace.logAction("Working", vector<string>(1, workspace->name));

        // Update needs according to action
        energy.level = getNeedUpdate(energy.level, DEC_ENERGY_WORK);
        productivity.level = getNeedUpdate(productivity.level, INC_PRODUCTIVITY_WORK);
        fun.level = getNeedUpdate(fun.level, DEC_FUN_WORK);

        // Calculate if Productive enough
        auto rulesEngine = e.world().get<RulesEngine>()->workEngine;
        auto highProductivityMembership = rulesEngine->getInputVariable(PRODUCTIVITY_INPUT)->getTerm("HIGH")->membership(productivity.level);

        // Check conditions whether action can be performed further
        // Stop Working if Productivity levels have been reached
        if (highProductivityMembership > 0.95)
            removeAction(&e, WORKING);
    }
}

void doGiveLecture(flecs::entity e, Trace &trace, GivingLecture const &lecture, Energy &energy, Productivity &productivity, Fun &fun)
{
    auto lhEntity = e.world().lookup(lecture.location.c_str());
    auto lecHall = lhEntity.get_mut<LectureHall>();

    // Must be at  lecture hall to give lecture - if have destination not yet arrived
    if (!e.has<Destination>())
    {
        vector<string> components = {lecture.lecName, lecture.location};
        auto currentTime = e.world().get<Clock>()->actualTime;

        // Waits until start time
        if (currentTime >= lecture.startTime && lecHall->attendance >= 5)
        {
            lecHall->hasLectureStarted = true;
            trace.logAction("GivingLecture", components);

            // Update needs according to action
            energy.level = getNeedUpdate(energy.level, DEC_ENERGY_LEC);
            productivity.level = getNeedUpdate(productivity.level, INC_PRODUCTIVITY_LEC);
            fun.level = getNeedUpdate(fun.level, DEC_FUN_LEC);

            // Check conditions whether action can be performed further
            // Stop GivingLecture when end of lecture
            if (currentTime > lecture.endTime)
                removeAction(&e, GIVING_LECTURE);
        }
        // Check conditions whether action can be performed further
        // Stop GivingLecture when at end of lecture or if not enough students arrive within sufficient time
        else if (currentTime > lecture.endTime || (currentTime - lecture.startTime > 0.25 && lecHall->attendance < 5))
            removeAction(&e, ATTENDING_LECTURE);
        else
            trace.logAction("WaitingLecture", components);
    }
}

void doAttendLecture(flecs::entity e, Trace &trace, AttendingLecture const &lecture, Energy &energy, Productivity &productivity, Fun &fun)
{
    auto lhEntity = e.world().lookup(lecture.location.c_str());
    auto lecHall = lhEntity.get_mut<LectureHall>();

    // Must be at lecture hall to attend lecture - if have destination not yet arrived
    if (!e.has<Destination>())
    {
        vector<string> components = {lecture.lecName, lecture.location};
        auto currentTime = e.world().get<Clock>()->actualTime;

        if (lecHall->hasLectureStarted && !lecHall->hasLectureEnded)
        {
            trace.logAction("AttendingLecture", components);

            // Update needs according to action
            energy.level = getNeedUpdate(energy.level, DEC_ENERGY_LEC);
            productivity.level = getNeedUpdate(productivity.level, INC_PRODUCTIVITY_LEC);
            fun.level = getNeedUpdate(fun.level, DEC_FUN_LEC);
        }
        // Check conditions whether action can be performed further
        // Stop AttendingLecture at end of lecture or professor takes longer than 15 mins to arrive
        else if (currentTime > lecture.endTime || currentTime - lecture.startTime > 0.25)
            removeAction(&e, ATTENDING_LECTURE);
        else
            trace.logAction("WaitingLecture", components);
    }
}

Position getUpdatedPosition(Direction dir, Position pos, int steps)
{
    switch (dir)
    {
    case UP:
        return {pos.x, pos.y - steps};
    case DOWN:
        return {pos.x, pos.y + steps};
    case LEFT:
        return {pos.x - steps, pos.y};
    case RIGHT:
        return {pos.x + steps, pos.y};
    default:
        return pos;
    }
}

void removeAction(flecs::entity *entity, ActionEnum action)
{
    switch (action)
    {
    case WALKING:
    {
        entity->remove<Walking>();
        entity->remove<Destination>();
        break;
    }
    case DRINKING_COFFEE:
    {
        // If coffee has been consumed, remove from inventory - else update inventory value
        auto coffee = entity->get<DrinkingCoffee>()->coffee;
        auto inventory = entity->get_mut<Inventory>();

        for (size_t i = 0; i < inventory->coffee.size(); i++)
        {
            if (coffee.name == inventory->coffee[i].name)
            {
                if (coffee.level < 0.05)
                    inventory->coffee.erase(inventory->coffee.begin() + i);
                else
                    inventory->coffee[i].level = coffee.level;
            }
        }

        entity->remove<DrinkingCoffee>();
        break;
    }
    case EATING:
    {
        // If food has been consumed, remove from inventory - else update inventory value
        auto food = entity->get<Eating>()->food;
        auto inventory = entity->get_mut<Inventory>();

        for (size_t i = 0; i < inventory->food.size(); i++)
        {
            if (food.name == inventory->food[i].name)
            {
                if (food.level < 0.05)
                    inventory->food.erase(inventory->food.begin() + i);
                else
                    inventory->food[i].level = food.level;
            }
        }

        entity->remove<Eating>();
        break;
    }
    case PLAYING_GAME:
        entity->remove<PlayingGame>();
        break;
    case ATTENDING_LECTURE:
    {
        // Reset lecture hall properties
        auto lecture = entity->get<AttendingLecture>();
        auto lecHallEntitiy = entity->world().lookup(lecture->location.c_str());
        auto lecHall = lecHallEntitiy.get_mut<LectureHall>();
        lecHall->attendance--;

        entity->remove<AttendingLecture>();
        break;
    }
    case STUDYING:
        entity->remove<Studying>();
        break;
    case GIVING_LECTURE:
    {
        // Reset lecture hall properties
        auto lecture = entity->get<GivingLecture>();
        auto lecHallEntitiy = entity->world().lookup(lecture->location.c_str());
        auto lecHall = lecHallEntitiy.get_mut<LectureHall>();
        lecHall->hasLectureEnded = true;

        entity->remove<GivingLecture>();
        break;
    }
    case WORKING:
        entity->remove<Working>();
        break;
    default:
        break;
    }

    if (entity->has<CurrentAction>())
        entity->remove<CurrentAction>();
}

void removeAllActions(flecs::entity *entity)
{
    if (entity->has<Walking>())
        removeAction(entity, WALKING);

    if (entity->has<DrinkingCoffee>())
        removeAction(entity, DRINKING_COFFEE);

    if (entity->has<Eating>())
        removeAction(entity, EATING);

    if (entity->has<PlayingGame>())
        removeAction(entity, PLAYING_GAME);

    if (entity->has<AttendingLecture>())
        removeAction(entity, ATTENDING_LECTURE);

    if (entity->has<Studying>())
        removeAction(entity, STUDYING);

    if (entity->has<GivingLecture>())
        removeAction(entity, GIVING_LECTURE);

    if (entity->has<Working>())
        removeAction(entity, WORKING);
}