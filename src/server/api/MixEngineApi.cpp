#include "server/api/MixEngineApi.h"

MixEngineApi::MixEngineApi(WebServer& server, MixEngine& mixEngine)
: _server(server),
  _mixEngine(mixEngine)
{
}

void MixEngineApi::begin() {
  start();
}

/**
 * POST /api/recipe/start
 */
void MixEngineApi::start() {
  _server.on("/api/recipe/start", HTTP_POST, [&]() {

    if (!_server.hasArg("name") || !_server.hasArg("totalMl")) {
      _server.send(400, "application/json",
                   "{\"error\":\"missing parameters\"}");
      return;
    }

    String name = _server.arg("name");
    unsigned long totalMl = _server.arg("totalMl").toInt();

    if (!_mixEngine.startRecipe(name, totalMl)) {
      _server.send(409, "application/json",
                   "{\"error\":\"ingredient not available\"}");
      return;
    }

    _server.send(200, "application/json",
                 "{\"status\":\"started\"}");
                 //_serverHandler.broadcastMachineStatus();
  });
}

