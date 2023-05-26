#include "world.ih"

World::World(flecs::world *world, string configFile)
    : d_mt(time(nullptr)),
      d_world(world)
{
    ifstream f(configFile);

    try
    {
        json j = json::parse(f);

        d_config = PopulationConfig{
            j["population"]["size"].get<size_t>(),
            j["population"]["studentProportion"].get<double>(),
            j["population"]["professorProportion"].get<double>(),
            j["population"]["adminProportion"].get<double>()};

        double sampleHours = j["meta"]["stepTime"]["hours"].get<double>();
        double sampleMinutes = j["meta"]["stepTime"]["minutes"].get<double>();
        double sampleSeconds = j["meta"]["stepTime"]["seconds"].get<double>();

        // Calculating simulation length
        d_stepLength = sampleHours + sampleMinutes / 60. + sampleSeconds / 3600.;
        d_simulationLength = ceil(24. / d_stepLength);

        // Set world size
        d_worldHeight = j["meta"]["size"]["height"].get<double>();
        d_worldWidth = j["meta"]["size"]["width"].get<double>();

        d_isWalkable = new bool *[d_worldHeight];

        for (size_t i = 0; i < d_worldHeight; ++i)
        {
            d_isWalkable[i] = new bool[d_worldWidth];

            // Setting to is walkable
            for (size_t j = 0; j < d_worldWidth; ++j)
                d_isWalkable[i][j] = true;
        }

        // Load Buildings
        loadBuildings(j["buildings"]);
    }
    catch (exception &ex)
    {
        cout << "Failed to read config file\n";
        throw ex;
    }

    // Load lectures
    loadCalendarTasks(configFile);

    // Create population
    initPopulation();
}

World::~World()
{
    for (size_t i = 0; i < d_worldHeight; i++)
        delete[] d_isWalkable[i];

    delete[] d_isWalkable;
}

void World::runSimulation()
{
    cout << "Starting Simulation\n";
    for (size_t i = 0; i < d_simulationLength; ++i)
    {
        d_world->progress();
        auto clock = d_world->get<Clock>();
        d_world->set<Clock>({clock->actualTime + clock->stepLength, clock->stepTime + 1, clock->stepLength});
    }

    cout << "Simulation complete\n";
}

double World::timeToSimulationStep(double time)
{
    return time / d_stepLength;
}

void World::loadCalendarTasks(std::string file)
{
    cout << "Loading calendar tasks\n";
    // Read configuration from file
    ifstream f(file);

    try
    {
        json j = json::parse(f);

        vector<CalendarTask> ictLectures;
        vector<CalendarTask> medLectures;
        vector<CalendarTask> lawLectures;
        vector<CalendarTask> busLectures;

        for (auto &lecConfig : j["calendar"]["ICT"])
        {
            CalendarTask lecture = getTaskFromJson(lecConfig);
            ictLectures.push_back(lecture);
            d_lectures.push_back(lecture);
        }

        for (auto &lecConfig : j["calendar"]["MEDICINE"])
        {
            CalendarTask lecture = getTaskFromJson(lecConfig);
            medLectures.push_back(lecture);
            d_lectures.push_back(lecture);
        }

        for (auto &lecConfig : j["calendar"]["LAW"])
        {
            CalendarTask lecture = getTaskFromJson(lecConfig);
            lawLectures.push_back(lecture);
            d_lectures.push_back(lecture);
        }

        for (auto &lecConfig : j["calendar"]["BUSINESS"])
        {
            CalendarTask lecture = getTaskFromJson(lecConfig);
            busLectures.push_back(lecture);
            d_lectures.push_back(lecture);
        }

        d_majorLectures.insert(make_pair(ICT, ictLectures));
        d_majorLectures.insert(make_pair(MEDICINE, medLectures));
        d_majorLectures.insert(make_pair(LAW, lawLectures));
        d_majorLectures.insert(make_pair(BUSINESS, busLectures));
    }
    catch (exception &ex)
    {
        cout << "Failed to read calendar tasks from file\n";
        throw ex;
    }
}

World::CalendarTask World::getTaskFromJson(json jsonObj) const
{
    return CalendarTask{
        jsonObj["name"].get<string>(),
        jsonObj["location"].get<string>(),
        jsonObj["start"].get<size_t>(),
        jsonObj["end"].get<size_t>()};
}

void World::loadBuildings(json jsonObj)
{
    cout << "Loading buildings\n";

    for (const auto &area : jsonObj)
    {
        if (area.contains("entrance"))
        {
            Position e = Position{
                area["entrance"]["x"].get<int>(),
                area["entrance"]["y"].get<int>()};

            Position p = Position{
                area["x"].get<int>(),
                area["y"].get<int>()};

            Building b = Building{
                area["name"].get<string>(),
                area["capacity"].get<size_t>(),
                area["width"].get<int>(),
                area["height"].get<int>(),
                p,
                vector<Object>(),
                e,
                vector<Room>()};

            definePerimeterOfArea(b, e);

            if (area.contains("rooms"))
            {
                for (const auto &room : area["rooms"])
                {
                    Position e = Position{
                        room["entrance"]["x"].get<int>(),
                        room["entrance"]["y"].get<int>()};

                    Position p = Position{
                        room["x"].get<int>(),
                        room["y"].get<int>()};

                    Room r = Room{
                        room["name"].get<string>(),
                        room["capacity"].get<size_t>(),
                        room["width"].get<int>(),
                        room["height"].get<int>(),
                        p,
                        vector<Object>(),
                        e,
                        b.name};

                    b.rooms.push_back(r);

                    definePerimeterOfArea(r, e);

                    for (const auto &ws : room["workspaces"])
                    {
                        Position p = Position{
                            ws["x"].get<int>(),
                            ws["y"].get<int>(),
                        };

                        Workspace w = Workspace{
                            ws["name"].get<string>(),
                            ws["width"].get<int>(),
                            ws["height"].get<int>(),
                            p,
                            ws["capacity"].get<size_t>(),
                            0};

                        r.objects.push_back(w);

                        // Add Workspace entity
                        d_world->entity(w.name.c_str())
                            .set<Workspace>(w);
                    }

                    // Add Room entity
                    d_world->entity(r.name.c_str())
                        .set<Room>(r);
                }
            }

            if (area.contains("lectureHalls"))
            {
                for (const auto &hall : area["lectureHalls"])
                {

                    Position e = Position{
                        hall["entrance"]["x"].get<int>(),
                        hall["entrance"]["y"].get<int>()};

                    Position p = Position{
                        hall["x"].get<int>(),
                        hall["y"].get<int>()};

                    LectureHall lh = LectureHall{
                        hall["name"].get<string>(),
                        hall["capacity"].get<size_t>(),
                        hall["width"].get<int>(),
                        hall["height"].get<int>(),
                        p,
                        vector<Object>(),
                        e,
                        b.name,
                        false,
                        false,
                        0};

                    b.rooms.push_back(lh);

                    definePerimeterOfArea(lh, e);

                    // Add LectureHall entity
                    d_world->entity(lh.name.c_str())
                        .set<LectureHall>(lh);
                }
            }

            if (area.contains("vendingMachines"))
            {
                for (const auto &vm : area["vendingMachines"])
                {
                    Position p = Position{
                        vm["x"].get<int>(),
                        vm["y"].get<int>(),
                    };

                    VendingMachine v = VendingMachine{
                        vm["name"].get<string>(),
                        vm["width"].get<int>(),
                        vm["height"].get<int>(),
                        p,
                        vector<Food>(),
                        false};

                    b.objects.push_back(v);

                    // Add VendingMachine entity
                    d_world->entity(v.name.c_str())
                        .set<VendingMachine>(v);
                }
            }

            if (area.contains("workspaces"))
            {
                for (const auto &ws : area["workspaces"])
                {
                    Position p = Position{
                        ws["x"].get<int>(),
                        ws["y"].get<int>(),
                    };

                    Workspace w = Workspace{
                        ws["name"].get<string>(),
                        ws["width"].get<int>(),
                        ws["height"].get<int>(),
                        p,
                        ws["capacity"].get<size_t>(),
                        0};

                    b.objects.push_back(w);

                    // Add Workspace entity
                    d_world->entity(w.name.c_str())
                        .set<Workspace>(w);
                }
            }

            // Add building entity
            d_world->entity(b.name.c_str())
                .set<Building>(b);
        }
        else
        {
            Position p = Position{
                area["x"].get<int>(),
                area["y"].get<int>(),
            };

            Area a = Area{
                area["name"].get<string>(),
                area["capacity"].get<size_t>(),
                area["width"].get<int>(),
                area["height"].get<int>(),
                p,
                vector<Object>()};

            if (area.contains("vendingMachines"))
            {
                for (const auto &vm : area["vendingMachines"])
                {
                    Position p = Position{
                        vm["x"].get<int>(),
                        vm["y"].get<int>(),
                    };

                    VendingMachine v = VendingMachine{
                        vm["name"].get<string>(),
                        vm["width"].get<int>(),
                        vm["height"].get<int>(),
                        p,
                        vector<Food>(),
                        false};

                    a.objects.push_back(v);

                    // Add Workspace entity
                    d_world->entity(v.name.c_str())
                        .set<VendingMachine>(v);
                }
            }
            else if (area.contains("coffee"))
            {
                for (const auto &cs : area["coffee"])
                {
                    Position p = Position{
                        cs["x"].get<int>(),
                        cs["y"].get<int>(),
                    };

                    CoffeeStall c = CoffeeStall{
                        cs["name"].get<string>(),
                        cs["width"].get<int>(),
                        cs["height"].get<int>(),
                        p,
                        false};

                    a.objects.push_back(c);

                    // Add Workspace entity
                    d_world->entity(c.name.c_str())
                        .set<CoffeeStall>(c);
                }
            }

            // Add area entity
            d_world->entity(a.name.c_str())
                .set<Area>(a);
        }
    }
}

void World::saveTraces(string dirPath)
{
    cout << "Saving Traces\n";

    // Get current time in string format
    stringstream ss;
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    string timeStr = ss.str();

    // Create new directory
    string newDirPath = dirPath + "/simulation_" + string(timeStr);
    mkdir(newDirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    saveActions(newDirPath);
    saveNeeds(newDirPath);
    savePositions(newDirPath);
    saveCalendar(newDirPath);
    saveMetaData(newDirPath);

    cout << "Traces saved in directory " << newDirPath << endl;
}

void World::saveNeeds(string dirPath)
{
    string needsDir = dirPath + "/needs";
    mkdir(needsDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    for (size_t idx = 0; idx < d_population.size(); ++idx)
    {
        auto const entity = d_population[idx];
        auto trace = entity.get<Trace>();

        ofstream needsFile;
        needsFile.open(needsDir + "/needs_" + to_string(idx) + ".csv");
        needsFile << "energy,productivity,fun,hunger\n";

        for (auto const &log : trace->needs())
            needsFile << log << endl;

        needsFile.close();
    }
}

void World::saveActions(string dirPath)
{
    ofstream tracesFile;
    tracesFile.open(dirPath + "/traces.txt");

    for (size_t idx = 0; idx < d_population.size(); ++idx)
    {
        auto const entity = d_population[idx];
        auto trace = entity.get<Trace>();

        // Save Action traces
        for (auto const &log : trace->trace())
            tracesFile << log;
        tracesFile << endl;
    }

    tracesFile.close();
}

void World::savePositions(string dirPath)
{
    ofstream posFile;
    posFile.open(dirPath + "/positions.txt");

    for (size_t idx = 0; idx < d_population.size(); ++idx)
    {
        auto const entity = d_population[idx];
        auto trace = entity.get<Trace>();
        auto positions = trace->positions();

        for (size_t i = 0; i < positions.size(); ++i)
        {
            posFile << positions[i].first << ':' << positions[i].second;

            if (i + 1 != positions.size())
                posFile << ',';
        }

        posFile << endl;
    }

    posFile.close();
}

void World::saveCalendar(string dirPath)
{
    ofstream calendarFile;
    calendarFile.open(dirPath + "/calendar.txt");

    for (size_t idx = 0; idx < d_population.size(); ++idx)
    {
        auto const entity = d_population[idx];
        if (entity.has<Calendar>())
        {
            auto calendar = entity.get<Calendar>();
            calendar->print(calendarFile);
        }
        calendarFile << endl;
    }

    calendarFile.close();
}

void World::saveMetaData(string dirPath)
{
    ofstream metaFile;
    metaFile.open(dirPath + "/meta.csv");
    metaFile << "id,type,driven,workaholic\n";

    for (size_t idx = 0; idx < d_population.size(); ++idx)
    {
        auto const entity = d_population[idx];
        double driven = entity.has<Student>() ? entity.get<Driven>()->level : 0;
        double workaholic = entity.has<Student>() ? 0 : entity.get<Workaholic>()->level;

        metaFile << idx;

        if (entity.has<Student>())
            metaFile << ",student,";
        else if (entity.has<Professor>())
            metaFile << ",professor,";
        else
            metaFile << ",admin,";

        metaFile << driven << ',' << workaholic << endl;
    }

    metaFile.close();
}

void World::initPopulation()
{
    cout << "Initialising Population\n";
    // Create Students
    size_t numStuds = d_config.size * d_config.studentProportion;
    boost::random::uniform_int_distribution<> dist(0, 3);
    for (size_t i = 0; i < numStuds; ++i)
    {
        int majorValue = dist(d_mt);
        MajorEnum major = static_cast<MajorEnum>(majorValue);
        createStudentEntity(major);
    }

    // Create Professors with Lectures
    for (auto lecture : d_lectures)
        createProfessorEntity(&lecture);

    // Create Professors with No Lectures
    int numProfs = d_config.size * d_config.professorProportion - d_lectures.size();
    while (numProfs > 0)
    {
        createProfessorEntity(nullptr);
        numProfs--;
    }

    // Create Admin
    size_t numAdmin = d_config.size * d_config.adminProportion;
    for (size_t i = 0; i < numAdmin; ++i)
        createAdminEntity();
}

void World::createStudentEntity(MajorEnum major)
{
    auto id = createEntity();
    auto student = d_world->entity(id);

    // Create Calendar for Student
    Calendar *calendar = new Calendar();
    fillStudentCalendar(major, calendar);

    // Traits
    double driven = getRandomDouble(-1, 1);

    student.set<Major>({major})
        .set<Driven>({driven})
        .set<Calendar>(*calendar)
        .add<Student>();

    d_population.push_back(student);
}

void World::createProfessorEntity(World::CalendarTask *lecture)
{
    auto id = createEntity();
    auto professor = d_world->entity(id);

    Calendar *calendar = new Calendar();
    string workLoc = getRandomWorkspace();
    calendar->addEvent(800, 1700, {"Work", workLoc, Calendar::WORK});

    // Traits
    double workaholic = getRandomDouble(-1, 1);

    if (lecture != nullptr)
        calendar->addEvent(lecture->start, lecture->end, {lecture->name, lecture->location, Calendar::GIVE_LECTURE});

    professor.set<Calendar>(*calendar)
        .set<Workaholic>({workaholic})
        .set<Professor>({workLoc});

    d_population.push_back(professor);
}

void World::createAdminEntity()
{
    auto id = createEntity();
    auto admin = d_world->entity(id);

    Calendar *calendar = new Calendar();
    string workLoc = getRandomWorkspace();
    calendar->addEvent(800, 1700, {"Work", workLoc, Calendar::WORK});

    // Traits
    double workaholic = getRandomDouble(-1, 1);

    admin.set<Calendar>(*calendar)
        .set<Workaholic>({workaholic})
        .set<Admin>({workLoc});
    d_population.push_back(admin);
}

flecs::entity_t World::createEntity()
{
    // Needs
    double energy = getRandomDouble(0, 1);
    double productivity = getRandomDouble(0, 1);
    double fun = getRandomDouble(0, 1);
    double hunger = getRandomDouble(0, 1);

    auto ety = d_world->entity()
                   .set<Energy>({energy})
                   .set<Productivity>({productivity})
                   .set<Fun>({fun})
                   .set<Hunger>({hunger})
                   .add<Inventory>()
                   .add<Trace>()
                   .add<Inactive>();

    return ety.id();
}

double World::getRandomDouble(double min, double max)
{
    // Setting the mean and standard deviation for the normal distribution
    double mean = (max + min) / 2.0;
    double stddev = (max - min) / 4.0;

    boost::random::normal_distribution<> normalDist(mean, stddev);

    // Scaling factor and shifting operation to adjust the range of values
    double scalingFactor = (max - min) / 2.0;
    double shiftingOperation = (max + min) / 2.0;

    double randomVar = scalingFactor * normalDist(d_mt) + shiftingOperation;
    return std::max(min, std::min(max, randomVar));
}

void World::fillStudentCalendar(MajorEnum major, Calendar *calendar)
{
    size_t lecturesCnt = 0;
    auto lectures = d_majorLectures.at(major);

    for (auto &lecture : lectures)
    {
        boost::random::uniform_int_distribution<> decision(0, 1);
        bool isAddLecture = decision(d_mt);
        if (isAddLecture)
        {
            Calendar::Task task = {lecture.name, lecture.location, Calendar::ATTEND_LECTURE};
            calendar->addEvent(lecture.start, lecture.end, task);

            lecturesCnt++;
        }
    }

    if (lecturesCnt == 0) 
    {
        boost::random::uniform_int_distribution<size_t> idxGen(0, lectures.size()-1);
        size_t idx = idxGen(d_mt);
        auto lecture = lectures[idx];

        Calendar::Task task = {lecture.name, lecture.location, Calendar::ATTEND_LECTURE};
        calendar->addEvent(lecture.start, lecture.end, task);
    }
}

void World::definePerimeterOfArea(Area const &area, Position const &entrance)
{
    // Set perimeter of building to not walkable
    for (int i = area.position.x; i < area.position.x + area.width; ++i)
        for (int j = area.position.y; j < area.position.y + area.height; ++j)
            if (i == area.position.x || i == area.position.x + area.width - 1 || j == area.position.y || j == area.position.y + area.height - 1)
                d_isWalkable[j][i] = false;

    // Setting entrance to walkable
    d_isWalkable[entrance.y][entrance.x] = true;
}

string World::getRandomWorkspace()
{
    boost::random::uniform_int_distribution<> dist(0, 4);
    size_t randomVal = dist(d_mt);
    string room;

    switch (randomVal)
    {
    case 0:
        room = "BUSSR";
    case 1:
        room = "ICTSR";
    case 2:
        room = "LAWSR";
    case 3:
    default:
        room = "MDSR";
    }
    
    auto workspaces = d_world->lookup(room.c_str()).get_mut<Room>()->objects;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, workspaces.size() - 1);
    size_t randIdx = dis(gen);

    return workspaces.at(randIdx).name;
}