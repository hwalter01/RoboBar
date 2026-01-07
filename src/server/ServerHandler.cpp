#include <ESPmDNS.h>
#include <server/ServerHandler.h>
#include <controller/pump/PumpManager.h>
#include <server/api/SystemApi.h>
#include <server/api/PumpApi.h>
#include <server/api/RecipeApi.h>
#include <controller/ingredient/IngredientController.h>
#include <controller/mix/MixEngine.h>
#include <LittleFS.h>

ServerHandler::ServerHandler(const char *ssid, const char *password, PumpManager &pumps, RecipeController &recipeController, IngredientController &ingredientController, MixEngine &mixEngine)
    : _ssid(ssid),
      _password(password),
      _server(80),
      _ws(81),
      _pumps(pumps),
      _recipes(recipeController),
      _ingredients(ingredientController),
      _mixEngine(mixEngine),
      _systemApi(_server, _ws, _pumps, _ingredients, _recipes, _mixEngine),
      _pumpApi(_server, _ws, _pumps),
      _recipeApi(_server, _recipes),
      _mixApi(_server, _mixEngine),
      _ingredientApi(_server, _ingredients)
{
}

void ServerHandler::begin()
{
  Serial.println("Verbinde mit WLAN...");
  WiFi.begin(_ssid, _password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWLAN verbunden!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("cocktailmixer"))
  {
    Serial.println("mDNS gestartet");
  }

  _ws.begin();
  _ws.onEvent([this](uint8_t client,
                   WStype_t type,
                   uint8_t* payload,
                   size_t length) {

  if (type == WStype_CONNECTED) {
    Serial.println("WebSocket client verbunden");

    // 🔴 HIER: Status sofort pushen
    sendInitialState(client);
  }
});

  _server.begin();
  setupRoutes();

  _mixEngine.setStatusCallback(
      [&](bool running, unsigned long remainingMs)
      {
        broadcastMachineStatus(running, remainingMs);
      });

  Serial.println("Webserver gestartet");
}

void ServerHandler::handle()
{
  _server.handleClient();
  _ws.loop();
  _mixEngine.loop();
}

void ServerHandler::addCorsHeaders()
{
  _server.sendHeader("Access-Control-Allow-Origin", "*");
  _server.sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  _server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void ServerHandler::setupRoutes()
{
  _server.onNotFound([&]()
                     {
    if (_server.method() == HTTP_OPTIONS)
    {
      addCorsHeaders();
      _server.send(204);
      return;
    }

    _server.send(404, "application/json", "{\"error\":\"not found\"}"); });

  // UI-Dateien
  _server.serveStatic("/", LittleFS, "/index.html");
  _server.serveStatic("/style.css", LittleFS, "/style.css");
  _server.serveStatic("/app.js", LittleFS, "/app.js");

  _systemApi.begin();
  _pumpApi.begin();
  _recipeApi.begin();
  _ingredientApi.begin();
  _mixApi.begin();

  
}

void ServerHandler::broadcastMachineStatus(
    bool running,
    unsigned long remainingMs)
{
  String json = "{";
  json += "\"type\":\"machineStatus\",";
  json += "\"running\":";
  json += running ? "true" : "false";
  json += ",";
  json += "\"remainingMs\":";
  json += String(remainingMs);
  json += "}";

  _ws.broadcastTXT(json);
}

void ServerHandler::sendInitialState(uint8_t client)
{
  // 1️⃣ Maschinenstatus
  String machine = "{";
  machine += "\"type\":\"machineStatus\",";
  machine += "\"running\":";
  machine += _mixEngine.isRunning() ? "true" : "false";
  machine += ",";
  machine += "\"remainingMs\":";
  machine += String(_mixEngine.remainingMs());
  machine += "}";

  _ws.sendTXT(client, machine);

  // 2️⃣ Pumpen-Kalibrierstatus
  String pumps = "{";
  pumps += "\"type\":\"pumpCalibration\",";
  pumps += "\"pumps\":[";

  for (uint8_t i = 0; i < MAX_PUMPS; i++) {
    if (i > 0) pumps += ",";
    pumps += "{";
    pumps += "\"id\":";
    pumps += i;
    pumps += ",";
    pumps += "\"calibrated\":";
    pumps += _pumps.isCalibrated(i) ? "true" : "false";
    pumps += ",";
    pumps += "\"mlPerSec\":";
    pumps += String(_pumps.getFlowRate(i), 2);
    pumps += "}";
  }

  pumps += "]}";

  _ws.sendTXT(client, pumps);
}
