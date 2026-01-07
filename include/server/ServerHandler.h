#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

#include "controller/pump/PumpManager.h"
#include <server/api/PumpApi.h>
#include <server/api/SystemApi.h>
#include <server/api/RecipeApi.h>
#include <server/api/IngredientsApi.h>
#include "api/MixEngineApi.h"

class ServerHandler
{
public:
  ServerHandler(const char *ssid, const char *password, PumpManager &pumps, RecipeController &recipeController, IngredientController &ingredientController, MixEngine &mixEngine);
  void begin();
  void handle();

private:
  const char *_ssid;
  const char *_password;

  WebServer _server;
  WebSocketsServer _ws;
  PumpManager &_pumps;
  RecipeController &_recipes;
  IngredientController &_ingredients;
  MixEngine &_mixEngine;

  SystemApi _systemApi;
  PumpApi _pumpApi;
  RecipeApi _recipeApi;
  IngredientsApi _ingredientApi;
  MixEngineApi _mixApi;

  void addCorsHeaders();
  void setupRoutes();

  void broadcastMachineStatus(bool running, unsigned long remainingMs);

  void sendInitialState(uint8_t client);
};

#endif
