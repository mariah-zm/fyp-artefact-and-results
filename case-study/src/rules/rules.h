#ifndef INCLUDED_RULES_H
#define INCLUDED_RULES_H

#include "fl/Headers.h"

#define ENERGY_INPUT        "Energy"
#define PRODUCTIVITY_INPUT  "Productivity"
#define HUNGER_INPUT        "Hunger"
#define FUN_INPUT           "Fun"

#define DRIVEN_INPUT        "Driven"
#define WORKAHOLIC_INPUT    "Workaholic"

#define UNI_OPEN_TIME_INPUT "UniversityOpen"
#define NEXT_TASK_INPUT     "NextTask"

#define WALK_OUTPUT         "Walk"
#define COFFEE_OUTPUT       "Coffee"
#define EAT_OUTPUT          "Eat"
#define PLAY_OUTPUT         "Play"
#define STUDY_WORK_OUTPUT   "Work"
#define ATTENDLEC_OUTPUT    "AttendLecture"
#define GIVELEC_OUTPUT      "GiveLecture"

#define SPAWN_OUTPUT        "Spawn"
#define DISABLE_OUTPUT      "Disable"

typedef struct RulesEngine 
{
    fl::Engine *coffeeEngine;
    fl::Engine *workEngine;
    fl::Engine *playEngine;
    fl::Engine *eatEngine;
    fl::Engine *lectureEngine;
    fl::Engine *spawningEngine;
    fl::Engine *disableNpcEngine;
} RulesEngine;

void initRulesEngine(RulesEngine *engine);
void destroyRulesEngine(RulesEngine *engine);

#endif