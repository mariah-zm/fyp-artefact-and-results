#include "trace.ih"

ostream &operator<<(ostream &out, Trace::ActionLog const &log)
{
    auto actions = log.actions;
    size_t numActions = actions.size();

    out << '[';

    for (size_t i = 0; i < numActions; ++i)
    {
        out << actions[i].actionName << '(';

        auto components = actions[i].components;
        size_t numComp = components.size();

        for (size_t j = 0; j < numComp; ++j)
        {
            out << components[j];

            if (j != numComp - 1)
                out << ',';
        }

        out << ')';

        if (i != numActions - 1)
            out << ',';
    }

    out << ']' << log.duration;

    return out;
}

ostream &operator<<(ostream &out, Trace::NeedsLog const &log)
{
    out << log.energy << ',' << log.productivity << ',' << log.fun << ',' << log.hunger;
    return out;
}

bool operator==(Trace::ActionDescription lhs, Trace::ActionDescription rhs)
{
    return lhs.actionName == rhs.actionName && lhs.components == rhs.components;
}

bool operator==(Trace::ActionLog lhs, Trace::ActionLog rhs)
{
    return lhs.actions == rhs.actions;
}

void Trace::logAction(string name)
{
    logAction(name, vector<string>{});
}

void Trace::logAction(string name, vector<string> components)
{
    ActionDescription desc;
    desc.actionName = name;
    desc.components = components;

    ActionLog log = {desc};

    // Increase duration of action in log if the last action in trace is exactly the same
    if (d_trace.size() > 0 && log == d_trace.back())
        ++d_trace.back().duration;
    // Otherwise add new log to trace
    else
        d_trace.push_back(log);
}

void Trace::logNeeds(double energy, double productivity, double fun, double hunger)
{
    NeedsLog log = {energy, productivity, fun, hunger};
    d_needs.push_back(log);
}

void Trace::logCompositeAction(string name, vector<string> components)
{
    ActionDescription desc;
    desc.actionName = name;
    desc.components = components;

    ActionLog currentLog = d_trace.back();
    ActionLog newLog = {currentLog, desc};

    d_trace.push_back(newLog);
}

void Trace::logPosition(Position pos)
{
    if (!d_positions.empty() && d_positions.back().first == pos)
        d_positions.back().second++;
    else 
        d_positions.push_back(pair(pos, 1));
}