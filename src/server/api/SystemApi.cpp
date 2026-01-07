#include "server/api/SystemApi.h"
#include <WebSocketsServer.h>
#include <controller/pump/PumpManager.h>
#include <controller/ingredient/IngredientController.h>
#include <controller/recipe/RecipeController.h>

SystemApi::SystemApi(WebServer &server,
                     WebSocketsServer &ws,
                     PumpManager &pumps,
                     IngredientController &ingredients,
                     RecipeController &recipes,
                     MixEngine &mixEngine)
    : _server(server),
      _ws(ws),
      _pumps(pumps),
      _ingredients(ingredients),
      _recipes(recipes),
      _mixEngine(mixEngine)
{
}
void SystemApi::begin()
{
  status();
  stop();
  factoryReset();
}

/**
 * MACHINE_STATUS
 * gives the current status of the whole mashine
 */
void SystemApi::status()
{
  _server.on("/api/status", HTTP_GET, [&]()
             {
    String json = "{";
    json += "\"running\": ";
    json += _pumps.isRunning() ? "true" : "false";
    json += ",";
    json += "\"remainingMs\": ";
    json += String(_pumps.remainingMs());
    json += "}";
    _server.send(200, "application/json", json); });
}

/**
 * NOT_AUS
 * stops all running pumps immediately
 */
void SystemApi::stop()
{
  _server.on("/api/stop", HTTP_POST, [&]()
             {
    _mixEngine.stop();
    _server.send(200, "application/json",
                 "{\"result\":\"stopped\"}"); });
}

void SystemApi::factoryReset()
{
  _server.on("/api/system/reset", HTTP_POST, [&]()
             {

    // 1️⃣ Alles stoppen
    _pumps.stopAll();

    // 2️⃣ Controller leeren
    _ingredients.clear();
    _recipes.clear();
    _pumps.clear();

    // 3️⃣ Optional: ESP reboot
    _server.send(200, "application/json",
                 "{\"status\":\"factory reset\"}");

    delay(300);
    ESP.restart(); });
}

void SystemApi::broadcastMachineStatus()
{
  String json = "{";
  json += "\"running\": ";
  json += _pumps.isRunning() ? "true" : "false";
  json += ",";
  json += "\"remainingMs\": ";
  json += String(_pumps.remainingMs());
  json += "}";

  _ws.broadcastTXT(json);
}