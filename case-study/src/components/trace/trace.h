#ifndef INCLUDED_TRACE_H
#define INCLUDED_TRACE_H

#include <string>
#include <vector>
#include <iosfwd>

#include "../environment/environment.h"

class Trace
{
public:
    struct ActionDescription
    {
        std::string actionName;
        std::vector<std::string> components;
    };

    struct ActionLog
    {
        ActionLog(ActionDescription a) : duration(1)
        {
            actions.push_back(a);
        }
        ActionLog(ActionLog l, Trace::ActionDescription a) : actions(l.actions), duration(1)
        {
            actions.push_back(a);
        }
        std::vector<Trace::ActionDescription> actions;
        size_t duration;
    };

    struct NeedsLog
    {
        double energy;
        double productivity;
        double fun;
        double hunger;
    };

private:
    std::vector<ActionLog> d_trace;
    std::vector<NeedsLog> d_needs;
    std::vector<std::pair<Position, size_t>> d_positions;

public:
    friend std::ostream &operator<<(std::ostream &out, ActionLog const &log);
    friend std::ostream &operator<<(std::ostream &out, NeedsLog const &log);
    friend bool operator==(ActionLog lhs, ActionLog rhs);
    friend bool operator==(ActionLog lhs, ActionLog rhs);

    std::vector<ActionLog> trace() const;
    std::vector<NeedsLog> needs() const;
    std::vector<std::pair<Position, size_t>> positions() const;
    void logAction(std::string name);
    void logAction(std::string name, std::vector<std::string> components);
    void logNeeds(double energy, double productivity, double fun, double hunger);
    void logPosition(Position pos);

private:
    void logCompositeAction(std::string name, std::vector<std::string> components);
};

inline std::vector<Trace::ActionLog> Trace::trace() const
{
    return d_trace;
}

inline std::vector<Trace::NeedsLog> Trace::needs() const
{
    return d_needs;
}

inline std::vector<std::pair<Position, size_t>> Trace::positions() const
{
    return d_positions;
}

#endif