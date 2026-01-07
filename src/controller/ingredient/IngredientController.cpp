#include "controller/ingredient/IngredientController.h"

IngredientController::IngredientController()
    : _count(0)
{
}

bool IngredientController::add(const String &name)
{
    if (_count >= MAX_SYSTEM_INGREDIENTS)
        return false;

    for (uint8_t i = 0; i < _count; i++)
    {
        if (_ingredients[i].name == name)
            return false;
    }

    Ingredient ing;
    ing.name = name;
    ing.pumpId = NO_PUMP_ASSIGNED;

    _ingredients[_count++] = ing;
    save();
    return true;
}

bool IngredientController::remove(const String &name)
{
    for (uint8_t i = 0; i < _count; i++)
    {
        if (_ingredients[i].name == name)
        {
            for (uint8_t j = i; j < _count - 1; j++)
            {
                _ingredients[j] = _ingredients[j + 1];
            }
            _count--;
            save();
            return true;
        }
    }
    return false;
}

bool IngredientController::assignPump(const String &name, int8_t pumpId)
{
    // 1) Wenn pumpId >= 0, zuerst andere Zutaten von dieser Pumpe lösen
    if (pumpId != NO_PUMP_ASSIGNED) {
        for (uint8_t j = 0; j < _count; j++) {
            if (_ingredients[j].name != name && _ingredients[j].pumpId == pumpId) {
                _ingredients[j].pumpId = NO_PUMP_ASSIGNED;
            }
        }
    }

    // 2) Zielzutat setzen
    for (uint8_t i = 0; i < _count; i++)
    {
        if (_ingredients[i].name == name)
        {
            _ingredients[i].pumpId = pumpId;
            save();
            return true;
        }
    }
    return false;
}


bool IngredientController::unassignPump(const String &name)
{
    return assignPump(name, NO_PUMP_ASSIGNED);
}

uint8_t IngredientController::count() const
{
    return _count;
}

const Ingredient *IngredientController::get(uint8_t index) const
{
    if (index >= _count)
        return nullptr;
    return &_ingredients[index];
}

const Ingredient *IngredientController::getByName(const String &name) const
{
    for (uint8_t i = 0; i < _count; i++)
    {
        if (_ingredients[i].name == name)
            return &_ingredients[i];
    }
    return nullptr;
}

bool IngredientController::isAvailable(const String& name) const {
  const Ingredient* ing = getByName(name);
  return ing && ing->pumpId != NO_PUMP_ASSIGNED;
}


bool IngredientController::areAllAvailable(
    const Recipe& recipe) const {

  for (uint8_t i = 0; i < recipe.ingredientCount; i++) {
    if (!isAvailable(recipe.ingredients[i].name)) {
      return false;
    }
  }
  return true;
}

void IngredientController::clear() {
  _count = 0;
  save();
}

void IngredientController::load() {
  Preferences prefs;
  prefs.begin("ingredients", true);   // RO

  // Anzahl laden
  _count = prefs.getUChar("count", 0);

  // Sicherheitscheck
  if (_count > MAX_SYSTEM_INGREDIENTS) {
    _count = 0;
    prefs.end();
    return;
  }

  // Ingredients laden
  for (uint8_t i = 0; i < _count; i++) {
    _ingredients[i].name =
      prefs.getString(("n" + String(i)).c_str(), "");
    _ingredients[i].pumpId =
      prefs.getChar(
        ("p" + String(i)).c_str(),
        NO_PUMP_ASSIGNED
      );
  }

  prefs.end();
}

void IngredientController::save() const {
  Preferences prefs;
  prefs.begin("ingredients", false);   // RW

  // Anzahl speichern
  prefs.putUChar("count", _count);

  // Ingredients speichern
  for (uint8_t i = 0; i < _count; i++) {
    prefs.putString(
      ("n" + String(i)).c_str(),
      _ingredients[i].name
    );
    prefs.putChar(
      ("p" + String(i)).c_str(),
      _ingredients[i].pumpId
    );
  }

  prefs.end();
}
