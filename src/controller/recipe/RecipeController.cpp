#include "controller/recipe/RecipeController.h"

RecipeController::RecipeController()
    : _recipeCount(0)
{
}

bool RecipeController::addRecipe(const Recipe &recipe)
{
    if (_recipeCount >= MAX_RECIPES)
        return false;

    // doppelte Namen verhindern
    for (uint8_t i = 0; i < _recipeCount; i++)
    {
        if (_recipes[i].name == recipe.name)
        {
            return false;
        }
    }

    _recipes[_recipeCount++] = recipe;
    save();
    return true;
}

bool RecipeController::deleteRecipe(const String &name)
{
    for (uint8_t i = 0; i < _recipeCount; i++)
    {
        if (_recipes[i].name == name)
        {

            // Array kompakt halten
            for (uint8_t j = i; j < _recipeCount - 1; j++)
            {
                _recipes[j] = _recipes[j + 1];
            }

            _recipeCount--;
            save();
            return true;
        }
    }
    return false;
}

uint8_t RecipeController::getRecipeCount() const
{
    return _recipeCount;
}

const Recipe *RecipeController::getRecipe(uint8_t index) const
{
    if (index >= _recipeCount)
        return nullptr;
    return &_recipes[index];
}

const Recipe *RecipeController::getRecipeByName(const String &name) const
{
    for (uint8_t i = 0; i < _recipeCount; i++)
    {
        if (_recipes[i].name == name)
        {
            return &_recipes[i];
        }
    }
    return nullptr;
}

void RecipeController::clear()
{
    _recipeCount = 0;
    save();
}

void RecipeController::load() {
  Preferences prefs;
  prefs.begin("recipes", true);   // RO

  _recipeCount = prefs.getUChar("count", 0);

  // Sicherheitscheck
  if (_recipeCount > MAX_RECIPES) {
    _recipeCount = 0;
    prefs.end();
    return;
  }

  for (uint8_t i = 0; i < _recipeCount; i++) {
    _recipes[i].name =
      prefs.getString(("name" + String(i)).c_str(), "");

    _recipes[i].ingredientCount =
      prefs.getUChar(("ic" + String(i)).c_str(), 0);

    if (_recipes[i].ingredientCount > MAX_RECIPE_INGREDIENTS) {
      _recipes[i].ingredientCount = 0;
    }

    for (uint8_t j = 0; j < _recipes[i].ingredientCount; j++) {
      _recipes[i].ingredients[j].name =
        prefs.getString(("i" + String(i) + "_" + String(j)).c_str(), "");
      _recipes[i].ingredients[j].percent =
        prefs.getUChar(("p" + String(i) + "_" + String(j)).c_str(), 0);
    }
  }

  prefs.end();
}

void RecipeController::save() const {
  Preferences prefs;
  prefs.begin("recipes", false);   // RW

  // Anzahl Rezepte
  prefs.putUChar("count", _recipeCount);

  for (uint8_t i = 0; i < _recipeCount; i++) {
    // Rezeptname
    prefs.putString(("name" + String(i)).c_str(), _recipes[i].name);

    // Anzahl Zutaten im Rezept
    prefs.putUChar(("ic" + String(i)).c_str(), _recipes[i].ingredientCount);

    // Zutaten speichern
    for (uint8_t j = 0; j < _recipes[i].ingredientCount; j++) {
      prefs.putString(
        ("i" + String(i) + "_" + String(j)).c_str(),
        _recipes[i].ingredients[j].name
      );
      prefs.putUChar(
        ("p" + String(i) + "_" + String(j)).c_str(),
        _recipes[i].ingredients[j].percent
      );
    }
  }

  prefs.end();
}
