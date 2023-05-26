#ifndef INCLUDED_WORLD_H
#define INCLUDED_WORLD_H

#include "../flecs/flecs.h"
#include "../helper/graph.h"
#include "../components/traits/traits.h"
#include "../components/trace/trace.h"
#include "../components/components/components.h"
#include "../components/calendar/calendar.h"
#include "../components/environment/environment.h"

#include <map>
#include <vector>
#include <boost/random.hpp>

#include <../nlohmann/json.hpp>

class World
{
private:
    struct CalendarTask
    {
        std::string name;
        std::string location;
        size_t start;
        size_t end;
    };

    struct PopulationConfig
    {
        size_t size;
        double studentProportion;
        double professorProportion;
        double adminProportion;
    };

    PopulationConfig d_config;

    double d_stepLength;
    size_t d_simulationLength;

    bool **d_isWalkable;

    boost::random::mt19937 d_mt;
    flecs::world *d_world;
    std::vector<flecs::entity> d_population;

    size_t d_worldWidth;
    size_t d_worldHeight;

    std::vector<CalendarTask> d_lectures;
    std::map<MajorEnum, std::vector<CalendarTask>> d_majorLectures;

public:
    World() = delete;
    World(flecs::world *world, std::string configFile);
    ~World();

    double stepLength() const;
    size_t worldWidth() const;
    size_t worldHeight() const;
    std::vector<flecs::entity> const population() const;
    bool **isWalkable() const;

    void runSimulation();
    void saveTraces(std::string dirPath);

private:
    double timeToSimulationStep(double time);

    void loadBuildings(nlohmann::json jsonObj);
    void loadCalendarTasks(std::string file);
    CalendarTask getTaskFromJson(nlohmann::json jsonObj) const;

    void initPopulation();

    flecs::entity_t createEntity();
    void createStudentEntity(MajorEnum major);
    void createProfessorEntity(CalendarTask *lecture);
    void createAdminEntity();

    void fillStudentCalendar(MajorEnum major, Calendar *calendar);

    double getRandomDouble(double min, double max);
    void definePerimeterOfArea(Area const &area, Position const &entrance);
    std::string getRandomWorkspace();

    void saveNeeds(std::string dirPath);
    void saveActions(std::string dirPath);
    void savePositions(std::string dirPath);
    void saveMetaData(std::string dirPath);
    void saveCalendar(std::string dirPath);
};

inline double World::stepLength() const
{
    return d_stepLength;
}

inline std::vector<flecs::entity> const World::population() const
{
    return d_population;
}

inline size_t World::worldWidth() const
{
    return d_worldWidth;
}

inline size_t World::worldHeight() const
{
    return d_worldHeight;
}

inline bool **World::isWalkable() const
{
    return d_isWalkable;
}

#endif