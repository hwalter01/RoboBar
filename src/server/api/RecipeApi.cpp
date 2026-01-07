#include <server/api/RecipeApi.h>

RecipeApi::RecipeApi(WebServer &server, RecipeController &recipeController)
    : _server(server), _recipeController(recipeController)
{
}

void RecipeApi::begin()
{
  addRecipe();
  deleteRecipe();
  listRecipes();
}

void RecipeApi::addRecipe()
{
  _server.on("/api/recipes", HTTP_POST, [&]()
             {
    String name = _server.arg("name");
    if (name.length() == 0)
    {
      _server.send(400, "application/json", "{\"error\":\"Name is required\"}");
      return;
    }

    Recipe recipe;
    recipe.name = name;

    for (uint8_t i = 0; i < MAX_RECIPE_INGREDIENTS; i++)
    {
      String ingredientName = _server.arg("ingredient" + String(i) + "_name");
      String ingredientPercentStr = _server.arg("ingredient" + String(i) + "_percent");
      if (ingredientName.length() == 0 || ingredientPercentStr.length() == 0)
        break;

      uint8_t percent = ingredientPercentStr.toInt();
      if (percent > 100)
        percent = 100;

      recipe.ingredients[recipe.ingredientCount].name = ingredientName;
      recipe.ingredients[recipe.ingredientCount].percent = percent;
      recipe.ingredientCount++;
    }

    if (_recipeController.addRecipe(recipe))
    {
      _server.send(201, "application/json", "{\"status\":\"Recipe added\"}");
    }
    else
    {
      _server.send(409, "application/json", "{\"error\":\"Recipe could not be added\"}");
    } });
}

void RecipeApi::listRecipes()
{
  _server.on("/api/recipes", HTTP_GET, [&]()
             {
    String json = "[";
    for (uint8_t i = 0; i < _recipeController.getRecipeCount(); i++)
    {
      const Recipe *recipe = _recipeController.getRecipe(i);
      if (i > 0)
        json += ",";
      json += "{";
      json += "\"name\":\"" + recipe->name + "\",";
      json += "\"ingredients\":[";
      for (uint8_t j = 0; j < recipe->ingredientCount; j++)
      {
        if (j > 0)
          json += ",";
        json += "{";
        json += "\"name\":\"" + recipe->ingredients[j].name + "\",";
        json += "\"percent\":" + String(recipe->ingredients[j].percent);
        json += "}";
      }
      json += "]";
      json += "}";
    }
    json += "]";
    _server.send(200, "application/json", json); });
}

void RecipeApi::deleteRecipe()
{
  _server.on("/api/recipes", HTTP_DELETE, [&]()
             {
    String name = _server.arg("name");
    if (name.length() == 0)
    {
      _server.send(400, "application/json", "{\"error\":\"Name is required\"}");
      return;
    }

    if (_recipeController.deleteRecipe(name))
    {
      _server.send(200, "application/json", "{\"status\":\"Recipe deleted\"}");
    }
    else
    {
      _server.send(404, "application/json", "{\"error\":\"Recipe not found\"}");
    } });
}