#include "controller/mix/MixEngine.h"

MixEngine::MixEngine(PumpManager &pumps,
                     IngredientController &ingredients,
                     RecipeController &recipes)
    : _pumps(pumps),
      _ingredients(ingredients),
      _recipes(recipes)
{
}

bool MixEngine::startRecipe(const String &recipeName,
                            unsigned long totalMl)
{
  const Recipe *recipe = _recipes.getRecipeByName(recipeName);
  if (!recipe)
    return false;

  if (!_ingredients.areAllAvailable(*recipe))
    return false;

  unsigned long maxDurationMs = 0;

  // 🔹 ERST ALLE PUMPEN BERECHNEN & STARTEN
  for (uint8_t i = 0; i < recipe->ingredientCount; i++)
  {
    const RecipeIngredient &ri = recipe->ingredients[i];
    const Ingredient *ing = _ingredients.getByName(ri.name);
    if (!ing)
      return false;

    if (!_pumps.isCalibrated(ing->pumpId))
      return false;

    float flow = _pumps.getFlowRate(ing->pumpId);
    if (flow <= 0)
      return false;

    unsigned long ml = (totalMl * ri.percent) / 100;
    unsigned long durationMs =
        (unsigned long)((ml / flow) * 1000.0f);

    _pumps.start(ing->pumpId, durationMs);

    if (durationMs > maxDurationMs)
      maxDurationMs = durationMs;
  }

  // 🔹 JETZT EINMAL DEN MIX-STATUS SETZEN
  _running = true;
  _endTime = millis() + maxDurationMs;
  _lastPush = 0;

  if (_statusCb)
    _statusCb(true, maxDurationMs); // initialer Push

  return true;
}


void MixEngine::setStatusCallback(
    std::function<void(bool, unsigned long)> cb)
{
  _statusCb = cb;
}



void MixEngine::loop()
{
  if (!_running)
    return;

  unsigned long now = millis();
  unsigned long remaining =
      (now < _endTime) ? (_endTime - now) : 0;

  // 🔴 Mix beendet
  if (remaining == 0)
  {
    _running = false;
    if (_statusCb)
      _statusCb(false, 0);
    return;
  }

  // 🔁 Status max. 10× pro Sekunde pushen
  if (now - _lastPush >= 100)
  {
    _lastPush = now;
    if (_statusCb)
      _statusCb(true, remaining);
  }
}

bool MixEngine::isRunning() const {
  return _running;
}

unsigned long MixEngine::remainingMs() const {
  if (!_running) return 0;
  unsigned long now = millis();
  return now < _endTime ? _endTime - now : 0;
}

void MixEngine::stop()
{
  if (!_running)
    return;

  _running = false;
  _endTime = 0;

  _pumps.stopAll();   // falls vorhanden

  if (_statusCb) {
    _statusCb(false, 0);   // 🔴 PUSH
  }
}
