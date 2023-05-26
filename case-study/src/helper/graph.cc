#include "graph.ih"

size_t Graph::addVertex(size_t x, size_t y)
{
    Waypoint wp = {x, y, d_vertices.size() - 1};
    d_vertices.push_back(wp);
    d_adjacencyList.push_back(vector<pair<size_t, double>>());

    return wp.idx;
}

void Graph::addEdge(Waypoint u, Waypoint v)
{
    double dx = u.x - v.x;
    double dy = v.y - v.y;
    double dist = sqrt(dx*dx + dy*dy);

    d_adjacencyList[u.idx].push_back(pair<size_t, double>(v.idx, dist));
    d_adjacencyList[v.idx].push_back(pair<size_t, double>(u.idx, dist));
}

bool operator==(Graph::Waypoint const &lhs, Graph::Waypoint const &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(Graph::Waypoint const &lhs, Graph::Waypoint const &rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}