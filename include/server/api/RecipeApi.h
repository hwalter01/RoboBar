#ifndef RECIPE_API_H
#define RECIPE_API_H

#include <WebServer.h>
#include <controller/recipe/RecipeController.h>

class RecipeApi
{
public:
  RecipeApi(WebServer &server, RecipeController& recipeController);
  void begin();

private:
  RecipeController& _recipeController;
  WebServer &_server;

  void addRecipe();
  void deleteRecipe();
  void listRecipes();
};

#endif