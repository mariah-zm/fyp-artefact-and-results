#include "components.ih"

bool operator!(Position const &pos)
{
    return pos.x < 0 || pos.y < 0;
}

bool operator==(Position const &lhs, Position const &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

std::ostream &operator<<(ostream &out, Position const &pos)
{
    out << pos.x << ' ' << pos.y;
    return out;
}

bool operator==(Food const &lhs, Food const &rhs)
{
    return lhs.name == rhs.name && lhs.level == rhs.level;
}