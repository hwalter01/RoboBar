#ifndef MIX_ENGINE_H
#define MIX_ENGINE_H

#include "controller/pump/PumpManager.h"
#include "controller/ingredient/IngredientController.h"
#include "controller/recipe/RecipeController.h"
#include <functional>

class MixEngine
{
public:
  MixEngine(PumpManager &pumps,
            IngredientController &ingredients,
            RecipeController &recipes);

  bool startRecipe(const String &recipeName,
                   unsigned long totalMl);

  void setStatusCallback(std::function<void(bool, unsigned long)> cb);

  void loop();

  bool isRunning() const;
  unsigned long remainingMs() const;

  void stop();

private:
  PumpManager &_pumps;
  IngredientController &_ingredients;
  RecipeController &_recipes;

  bool _running = false;
  unsigned long _endTime = 0;

  unsigned long _lastPush = 0;

  std::function<void(bool, unsigned long)> _statusCb;

  void notifyStatus();

  
};

#endif
