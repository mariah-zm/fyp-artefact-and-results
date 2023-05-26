#include "main.ih"

void checkIfSpawn(flecs::entity e, Inactive const &inactive, Calendar const &calendar, Trace &trace);
void checkCalendar(flecs::entity e, Active const &active, Calendar const &calendar);
void checkDestination(flecs::entity e, Destination &des, Position const &loc);

void checkInventory(flecs::entity e, Active const &active, Inventory &inventory);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "-- USAGE INFORMATION --------------" << endl;
        cout << "./university path/to/config/file [path/to/results/directory]" << endl;
        cout << "You must provide at least one argument" << endl;
        return -1;
    }
    else if (argc > 3)
    {
        cout << "-- USAGE INFORMATION --------------" << endl;
        cout << "./university path/to/config/file [path/to/results/directory]" << endl;
        cout << "You provided too many arguments" << endl;
        return -1;
    }
    
    // Initialise flecs world and world object
    flecs::world world;
    World worldObj = World(&world, argv[1]);

    // system that takes action probabilities and performs the one with highest probablity?

    // Initialise Clock
    Clock clock = {0, 0, worldObj.stepLength()};
    world.component<Clock>();
    world.set<Clock>(clock);

    // Initialise Environment
    Environment env = {worldObj.worldWidth(), worldObj.worldHeight(), worldObj.isWalkable()};
    world.component<Environment>();
    world.set<Environment>(env);

    // Initialise Fuzzy Logic Rule Engine
    RulesEngine rulesEngine;
    initRulesEngine(&rulesEngine);
    world.component<RulesEngine>();
    world.set<RulesEngine>(rulesEngine);

    // Register Spawning System
    world.system<Inactive const, Calendar const, Trace>()
        .each(checkIfSpawn);

    // Register Inventory System
    world.system<Active const, Inventory>()
        .each(checkInventory);

    // Register Needs Systems
    world.system<Active const, Energy, Inventory const>()
        .each(checkEnergy);

    world.system<Active const, Hunger, Inventory const>()
        .each(checkHunger);

    world.system<Active const, Productivity, Driven const>()
        .each(checkStudentProductivity);

    world.system<Active const, Productivity, Workaholic const>()
        .each(checkProductivity);

    world.system<Active const, Fun>()
        .each(checkFun);

    // Register Calendar System
    world.system<Active const, Calendar const>()
        .each(checkCalendar);

    // Register Destination System
    world.system<Destination, Position const>()
        .each(checkDestination);

    // Register Actions Systems

    world.system<Trace, Walking, Destination const, Position, Energy>()
        .each(doWalk);

    world.system<Trace, PlayingGame const, Fun>()
        .each(doPlayGame);

    world.system<Trace, DrinkingCoffee, Energy>()
        .each(doDrink);

    world.system<Trace, Eating, Hunger, Energy>()
        .each(doEat);

    world.system<Trace, Working const, Energy, Productivity, Fun>()
        .each(doWork);

    world.system<Trace, Studying const, Energy, Productivity, Fun>()
        .each(doStudy);

    world.system<Trace, GivingLecture, Energy, Productivity, Fun>()
        .each(doGiveLecture);

    world.system<Trace, AttendingLecture, Energy, Productivity, Fun>()
        .each(doAttendLecture);

    // Register Trace System

    world.system<Active const, Trace, Energy const, Productivity const, Fun const, Hunger const, Position const>()
        .each([](flecs::entity e, Active const &active, Trace &trace, Energy const &energy, Productivity const &productivity, Fun const &fun, Hunger const &hunger, Position const &pos)
              { 
                trace.logNeeds(energy.level, productivity.level, fun.level, hunger.level); 
                trace.logPosition(pos); });

    // Register LectureHall reset
    world.system<LectureHall>()
        .kind(flecs::OnUpdate)
        .each([](flecs::entity e, LectureHall &hall)
              { if (hall.hasLectureStarted && hall.hasLectureEnded)
                {
                    hall.hasLectureStarted = false;
                    hall.hasLectureEnded = false;
                    hall.attendance = 0;
                } });

    // Run Simulation
    worldObj.runSimulation();

    // Save Traces to file
    if (argc > 2)
        worldObj.saveTraces(argv[2]);

    destroyRulesEngine(&rulesEngine);
}

void checkIfSpawn(flecs::entity e, Inactive const &inactive, Calendar const &calendar, Trace &trace)
{
    double currentTime = e.world().get<Clock>()->actualTime;
    auto rulesEngine = e.world().get<RulesEngine>()->spawningEngine;

    // Read Calendar
    auto nextTask = calendar.nextScheduledTask(currentTime);
    double nextTaskTime = nextTask ? nextTask->start : -1;

    // Set driven/workaholic
    double driven = e.has<Driven>() ? e.get<Driven>()->level : e.get<Workaholic>()->level;

    // Set Input variables
    if (nextTaskTime < 0)
        rulesEngine->setInputValue(NEXT_TASK_INPUT, 24);
    else
        rulesEngine->setInputValue(NEXT_TASK_INPUT, nextTaskTime - currentTime);

    rulesEngine->setInputValue(UNI_OPEN_TIME_INPUT, currentTime);
    rulesEngine->setInputValue(DRIVEN_INPUT, driven);
    rulesEngine->process();

    // Decide if to spawn
    auto doSpawn = rulesEngine->getOutputVariable(SPAWN_OUTPUT);
    doSpawn->defuzzify();
    double spawnDecision = doSpawn->getValue();

    if (spawnDecision > 0.5)
    {
        e.remove<Inactive>();
        e.add<Active>();

        auto spawnArea = e.world().lookup("UNI_GATE_01").get<Area>();
        Position pos = getRandomLocationInArea(*spawnArea);
        e.set<Position>(pos);
    }
    else
        trace.logAction("Inactive");
}

void checkCalendar(flecs::entity e, Active const &active, Calendar const &calendar)
{
    double currentTime = e.world().get<Clock>()->actualTime;
    auto rulesEngine = e.world().get<RulesEngine>()->disableNpcEngine;

    // Decide if to disable NPC
    rulesEngine->setInputValue(UNI_OPEN_TIME_INPUT, currentTime);
    rulesEngine->process();

    auto doDisable = rulesEngine->getOutputVariable(DISABLE_OUTPUT);
    doDisable->defuzzify();
    double disableDecision = doDisable->getValue();

    if (disableDecision > 0.7)
    {
        e.remove<Active>();
        removeAllActions(&e);
        e.set<Destination>({"UNI_GATE_02"});
    }
    // Otherwise decide next action if there is a task
    else
    {
        // Read Calendar
        auto nextTask = calendar.nextScheduledTask(currentTime);

        if (nextTask)
        {
            double currentTime = e.world().get<Clock>()->actualTime;
            auto rulesEngine = e.world().get<RulesEngine>()->spawningEngine;
            auto isTaskSoonMembership = rulesEngine->getInputVariable(NEXT_TASK_INPUT)->getTerm("NOW")->membership(nextTask->start - currentTime);
            double driven = e.has<Driven>() ? e.get<Driven>()->level : e.get<Workaholic>()->level;

            if (isTaskSoonMembership * (1 + driven) > 0.4)
            {
                switch (nextTask->task.taskType)
                {
                case Calendar::ATTEND_LECTURE:
                {
                    if (!e.has<AttendingLecture>())
                    {
                        // Decide if to go to lecture
                        auto rulesEngine = e.world().get<RulesEngine>()->lectureEngine;
                        rulesEngine->setInputValue(ENERGY_INPUT, e.get<Energy>()->level);
                        rulesEngine->setInputValue(FUN_INPUT, e.get<Fun>()->level);
                        rulesEngine->setInputValue(DRIVEN_INPUT, e.get<Driven>()->level);
                        rulesEngine->process();

                        auto outputLecture = rulesEngine->getOutputVariable(ATTENDLEC_OUTPUT);
                        outputLecture->defuzzify();
                        double lectureProbability = outputLecture->getValue();

                        if (doReplaceAction(&e, lectureProbability))
                        {
                            e.set<AttendingLecture>({nextTask->task.task, nextTask->task.location, nextTask->start, nextTask->end});
                            e.set<CurrentAction>({ATTENDING_LECTURE, 2});
                            e.set<Destination>({nextTask->task.location});
                        }
                    }

                    break;
                }
                case Calendar::GIVE_LECTURE:
                {
                    if (!e.has<GivingLecture>())
                    {
                        e.set<GivingLecture>({nextTask->task.task, nextTask->task.location, nextTask->start, nextTask->end});
                        e.set<CurrentAction>({GIVING_LECTURE, 2});
                        e.set<Destination>({nextTask->task.location});
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}

void checkDestination(flecs::entity e, Destination &des, Position const &pos)
{
    // Set path to destination if not set
    if (des.path.empty())
    {
        string locationDes = des.location;
        auto areaEntity = e.world().lookup(locationDes.c_str());

        if (areaEntity.has<Area>())
        {
            auto area = areaEntity.get<Area>();
            Position destinationPos = getRandomLocationInArea(*area);
            des.path.push_back(destinationPos);
        }
        else if (areaEntity.has<Building>())
        {
            auto building = areaEntity.get<Building>();
            Position destinationPos = getRandomLocationInArea(*building);
            des.path.push_back(destinationPos);
        }
        else if (areaEntity.has<Workspace>())
        {
            auto workspace = areaEntity.get<Workspace>();
            Position destinationPos = getRandomLocationInArea(workspace->position, workspace->width, workspace->height);
            des.path.push_back(destinationPos);
        }
        else
        {
            auto room = areaEntity.has<Room>() ? areaEntity.get<Room>() : areaEntity.get<LectureHall>();
            Position destinationPos = getRandomLocationInArea(*room);
            des.path.push_back(destinationPos);
        }

        e.add<Walking>();
        e.set<CurrentAction>({WALKING, 1});
    }
    else if (des.path.front() == pos) // Check if reached destination
    {
        // If final destination point has been reached, remove component
        if (des.path.size() == 1)
        {
            // Increase attendance if student has arrived at lecture hall
            if (e.has<AttendingLecture>())
            {
                auto lecture = e.get<AttendingLecture>();
                auto lecHallEntitiy = e.world().lookup(lecture->location.c_str());
                auto lecHall = lecHallEntitiy.get_mut<LectureHall>();
                lecHall->attendance++;
            }
            else if (e.has<Studying>())
            {
                string wsName = e.get<Studying>()->workspaceName;
                auto workspace = e.world().lookup(wsName.c_str()).get_mut<Workspace>();
                workspace->occupancy++;
            }
            else if (e.has<Working>())
            {
                string wsName = e.get<Working>()->workspaceName;
                auto workspace = e.world().lookup(wsName.c_str()).get_mut<Workspace>();
                workspace->occupancy++;
            }

            if (e.has<Walking>())
                e.remove<Walking>();

            e.remove<Destination>();
        }
        else
            des.path.erase(des.path.begin());

        if (!e.has<Active>())
            e.add<Inactive>();
    }
}

void checkInventory(flecs::entity e, Active const &active, Inventory &inventory)
{
    if (inventory.food.empty())
    {
        inventory.food.push_back({"Chocolate Bar", 1});
        inventory.food.push_back({"Potato Chips", 1});
    }

    if (inventory.coffee.empty())
        inventory.coffee.push_back({"Coffee", 1});
}
