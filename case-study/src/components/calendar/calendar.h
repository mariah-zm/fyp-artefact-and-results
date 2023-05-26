#ifndef INCLUDED_CALENDAR_H
#define INCLUDED_CALENDAR_H

#include "../../flecs/flecs.h"

#include <string>
#include <iostream>
#include <fstream>

class Calendar
{
public:
    typedef enum TaskTypeEnum
    {
        ATTEND_LECTURE,
        GIVE_LECTURE,
        WORK
    } TaskTypeEnum;

    typedef struct Task
    {
        std::string task;
        std::string location;
        TaskTypeEnum taskType;
    } Task;

    typedef struct Interval
    {
        Interval(size_t s, size_t e, Task t) : task(t)
        {
            start = ((int)s / 100) + (s % 100) / 60;
            end = ((int)e / 100) + (e % 100) / 60;
        }

        double start;
        double end;
        Task task;
    } Interval;

private:
    typedef struct Node
    {
        Node(Interval i) : interval(i), maxEnd(i.end), left(nullptr), right(nullptr) {}
        Node(Node const &other) : interval(other.interval), maxEnd(other.maxEnd)
        {
            left = other.left ? new Node(*other.left) : nullptr;
            right = other.right ? new Node(*other.right) : nullptr;
        }

        Interval interval;
        double maxEnd;
        Node *left;
        Node *right;
    } Node;

    Node *d_root;

public:
    Calendar();
    Calendar(Calendar const &other);
    Calendar(size_t start, size_t end, Task task);
    ~Calendar();

    Interval const *nextScheduledTask(double currentTime) const;

    void print(std::ofstream &out) const;
    void addEvent(size_t start, size_t end, Task task);
    Task const *searchCalendar(double start, double end) const;

private:
    void destroyNode(Node *node);
    void print(std::ofstream &out, Node *root) const;
    Node *insertInterval(Calendar::Node *root, Interval interval);
    Interval *searchInterval(Node *root, double start, double end) const;
    Interval *nextInterval(Calendar::Node *root, double start) const;
};

inline Calendar::Calendar()
    : d_root(nullptr)
{
}

inline Calendar::Calendar(Calendar const &other)
{
    if (other.d_root == nullptr)
    {
        d_root = nullptr;
    }
    else
    {
        d_root = new Node(*other.d_root);
    }
}

inline Calendar::Calendar(size_t start, size_t end, Calendar::Task task)
{
    addEvent(start, end, task);
}

inline Calendar::~Calendar()
{
    destroyNode(d_root);
}

inline void Calendar::print(std::ofstream &out) const
{
    print(out, d_root);
}

#endif