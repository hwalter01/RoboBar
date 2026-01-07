#ifndef SYSTEM_API_H
#define SYSTEM_API_H

#include <WebServer.h>
#include <WebSocketsServer.h>
#include <controller/pump/PumpManager.h>
#include <controller/ingredient/IngredientController.h>
#include <controller/recipe/RecipeController.h>
#include <controller/mix/MixEngine.h>

class SystemApi
{
public:
    SystemApi(WebServer &server,
              WebSocketsServer &ws,
              PumpManager &pumps,
              IngredientController &ingredients,
              RecipeController &recipes,
              MixEngine &mixEngine);
    void begin();

private:
    WebServer &_server;
    WebSocketsServer &_ws;
    PumpManager &_pumps;
    IngredientController &_ingredients;
    RecipeController &_recipes;
    MixEngine &_mixEngine;

    void status();
    void stop();

    void factoryReset();

    void broadcastMachineStatus();
};

#endif