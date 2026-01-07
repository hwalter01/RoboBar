#ifndef RECIPE_H
#define RECIPE_H

#include <Arduino.h>
#include "model/Ingredient.h"

#define MAX_RECIPE_INGREDIENTS 8

struct RecipeIngredient {
  String name;
  uint8_t percent;
};

struct Recipe {
  String name;
  uint8_t ingredientCount = 0;
  RecipeIngredient ingredients[MAX_RECIPE_INGREDIENTS];
};

#endif
