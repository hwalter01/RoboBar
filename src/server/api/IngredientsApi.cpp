#include "server/api/IngredientsApi.h"

IngredientsApi::IngredientsApi(WebServer& server,
                               IngredientController& ingredients)
: _server(server),
  _ingredients(ingredients)
{
}

void IngredientsApi::begin() {
  add();
  list();
  remove();
  assign();
}

void IngredientsApi::add() {
  _server.on("/api/ingredients", HTTP_POST, [&]() {
    if (!_server.hasArg("name")) {
      _server.send(400, "application/json",
                   "{\"error\":\"missing name\"}");
      return;
    }

    if (!_ingredients.add(_server.arg("name"))) {
      _server.send(409, "application/json",
                   "{\"error\":\"ingredient exists\"}");
      return;
    }

    _server.send(201, "application/json",
                 "{\"status\":\"ingredient added\"}");
  });
}

void IngredientsApi::list() {
  _server.on("/api/ingredients", HTTP_GET, [&]() {
    String json = "[";

    bool first = true;
    for (uint8_t i = 0; i < _ingredients.count(); i++) {
      const Ingredient* ing = _ingredients.get(i);
      if (!ing) continue;

      if (!first) json += ",";
      first = false;

      json += "{";
      json += "\"name\":\"" + ing->name + "\",";
      json += "\"pumpId\":" + String(ing->pumpId);
      json += "}";
    }

    json += "]";
    _server.send(200, "application/json", json);
  });
}

void IngredientsApi::remove() {
  _server.on("/api/ingredients", HTTP_DELETE, [&]() {
    if (!_server.hasArg("name")) {
      _server.send(400, "application/json",
                   "{\"error\":\"missing name\"}");
      return;
    }

    if (!_ingredients.remove(_server.arg("name"))) {
      _server.send(404, "application/json",
                   "{\"error\":\"not found\"}");
      return;
    }

    _server.send(200, "application/json",
                 "{\"status\":\"ingredient deleted\"}");
  });
}

void IngredientsApi::assign() {
  _server.on("/api/ingredients/assign", HTTP_POST, [&]() {
    if (!_server.hasArg("name") || !_server.hasArg("pumpId")) {
      _server.send(400, "application/json",
                   "{\"error\":\"missing params\"}");
      return;
    }

    if (!_ingredients.assignPump(
          _server.arg("name"),
          _server.arg("pumpId").toInt())) {
      _server.send(404, "application/json",
                   "{\"error\":\"ingredient not found\"}");
      return;
    }

    _server.send(200, "application/json",
                 "{\"status\":\"assigned\"}");
  });
}
