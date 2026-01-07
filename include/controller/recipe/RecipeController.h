#ifndef RECIPE_CONTROLLER_H
#define RECIPE_CONTROLLER_H

#include <Arduino.h>
#include "model/Recipe.h"
#include <Preferences.h>

#define MAX_RECIPES 10

class RecipeController
{
public:
  RecipeController();

  bool addRecipe(const Recipe &recipe);
  bool deleteRecipe(const String &name);

  uint8_t getRecipeCount() const;
  const Recipe* getRecipe(uint8_t index) const;
  const Recipe* getRecipeByName(const String &name) const;

  void clear();

  void load();
  void save() const; 

private:
  Recipe _recipes[MAX_RECIPES];
  uint8_t _recipeCount;
};

#endif
