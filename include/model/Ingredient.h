#ifndef INGREDIENT_H
#define INGREDIENT_H

#include <Arduino.h>

#define NO_PUMP_ASSIGNED -1

struct Ingredient {
  String name;
  int8_t pumpId = NO_PUMP_ASSIGNED;
};

#endif