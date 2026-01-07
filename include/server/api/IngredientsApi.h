#ifndef INGREDIENTS_API_H
#define INGREDIENTS_API_H

#include <WebServer.h>
#include "controller/ingredient/IngredientController.h"

class IngredientsApi {
public:
  IngredientsApi(WebServer& server, IngredientController& ingredients);
  void begin();

private:
  WebServer& _server;
  IngredientController& _ingredients;

  void add();
  void list();
  void remove();
  void assign();
};

#endif
