#include "environment.ih"

Position getRandomLocationInArea(Area const &area)
{
    // Seed the random number generator with the current time
    static boost::random::mt19937 rng(static_cast<unsigned>(time(nullptr)));

    // Define a distribution for the x and y coordinates
    boost::random::uniform_int_distribution x_dist(area.position.x + 1, area.position.x + area.width - 2);
    boost::random::uniform_int_distribution y_dist(area.position.y + 1, area.position.y + area.height - 2);

    int rand_x = x_dist(rng);
    int rand_y = y_dist(rng);

    return Position{rand_x, rand_y};
}

Position getRandomLocationInArea(Position const &pos, int width, int height)
{
    // Seed the random number generator with the current time
    static boost::random::mt19937 rng(static_cast<unsigned>(time(nullptr)));

    // Define a distribution for the x and y coordinates
    boost::random::uniform_int_distribution x_dist(pos.x, pos.x + width - 1);
    boost::random::uniform_int_distribution y_dist(pos.y, pos.y + height - 1);

    int rand_x = x_dist(rng);
    int rand_y = y_dist(rng);

    return Position{rand_x, rand_y};
}

bool isPositionInArea(Position const pos, Area const &area)
{
    return pos.x >= area.position.x && pos.x < area.position.x + area.width && pos.y >= area.position.y && pos.y < area.position.y + area.height;
}

bool isPositionInArea(Position const pos, Position const areaPos, int width, int height)
{
    return pos.x >= areaPos.x && pos.x < areaPos.x + width && pos.y >= areaPos.y && pos.y < areaPos.y + height;
}
