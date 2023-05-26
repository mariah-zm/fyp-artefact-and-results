#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H

// Effect of Time on Needs
#define DEC_ENERGY          -0.0001
#define DEC_PRODUCTIVIY     -0.0001
#define DEC_FUN             -0.0001
#define DEC_HUNGER          -0.0001

// Effect of Walking on Needs
#define DEC_ENERGY_WALK     -0.0005

// Effect of DrinkingCoffee on Needs
#define INC_ENERGY_COFFEE   0.05

// Effect of Eating on Needs
#define INC_ENERGY_EAT      0.003
#define INC_HUNGER_EAT      0.004

// Effect of PlayingGame on Needs
#define INC_FUN_PLAY        0.03

// Effect of Studying on Needs
#define DEC_ENERGY_STUDY        -0.001
#define DEC_FUN_STUDY           -0.002
#define INC_PRODUCTIVITY_STUDY  0.001

// Effect of Working on Needs
#define DEC_ENERGY_WORK         -0.003
#define DEC_FUN_WORK            -0.004
#define INC_PRODUCTIVITY_WORK   0.001

// Effect of AttendingLecture/GivingLecture on Needs
#define DEC_ENERGY_LEC         -0.0003
#define DEC_FUN_LEC            -0.0004
#define INC_PRODUCTIVITY_LEC   0.001

#endif