#ifndef INCLUDED_TRAITS_H
#define INCLUDED_TRAITS_H

#include <cstddef>
#include <vector>

typedef struct Trait
{
} Trait;

enum MajorEnum
{
    ICT,
    LAW,
    MEDICINE,
    BUSINESS
};

typedef struct Major : public Trait
{
    Major() {}
    Major(MajorEnum m) : major(m) {}
    MajorEnum major;
} Major;

typedef struct Workaholic : public Trait
{
    Workaholic() {}
    Workaholic(double l) : level(l) {}
    double level;
} Workaholic;

typedef struct Driven : public Trait
{
    Driven() {}
    Driven(double l) : level(l) {}
    double level;
} Driven;

#endif