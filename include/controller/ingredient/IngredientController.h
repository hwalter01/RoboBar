#ifndef INGREDIENT_CONTROLLER_H
#define INGREDIENT_CONTROLLER_H

#include <Arduino.h>
#include "model/Ingredient.h"
#include <model/Recipe.h>
#include <Preferences.h>

#define MAX_SYSTEM_INGREDIENTS 8

class IngredientController {
public:
  IngredientController();

  bool add(const String& name);
  bool remove(const String& name);

  bool assignPump(const String& name, int8_t pumpId);
  bool unassignPump(const String& name);

  uint8_t count() const;
  const Ingredient* get(uint8_t index) const;
  const Ingredient* getByName(const String& name) const;

  bool isAvailable(const String& name) const;
  bool areAllAvailable(const Recipe& recipe) const;

  void clear();
  void load();
  void save() const;

private:
  Ingredient _ingredients[MAX_SYSTEM_INGREDIENTS];
  uint8_t _count;

  
};

#endif
