#ifndef INCLUDED_GRAPH_H
#define INCLUDED_GRAPH_H

#include <vector>
#include <algorithm>

class Graph
{
public:
    struct Waypoint
    {
        size_t x, y, idx;
    };

private:
    std::vector<std::vector<std::pair<size_t, double>>> d_adjacencyList;
    std::vector<Waypoint> d_vertices;

public:
    size_t size() const;
    size_t addVertex(size_t x, size_t y);
    void addEdge(Waypoint u, Waypoint v);
    std::vector<Waypoint> const &waypoints() const;
    std::vector<std::pair<size_t, double>> const &getAdjacentVertices(Waypoint w);

    friend bool operator==(Waypoint const &lhs, Waypoint const &rhs);
    friend bool operator!=(Waypoint const &lhs, Waypoint const &rhs);
};

inline std::vector<Graph::Waypoint> const &Graph::waypoints() const
{
    return d_vertices;
}

inline std::vector<std::pair<size_t, double>> const &Graph::getAdjacentVertices(Graph::Waypoint w)
{
    return d_adjacencyList[w.idx];
}

inline size_t Graph::size() const
{
    return d_vertices.size();
}

#endif