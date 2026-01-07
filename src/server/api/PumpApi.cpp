#include <controller/pump/PumpManager.h>
#include "server/api/PumpApi.h"
#include <server/api/SystemApi.h>

PumpApi::PumpApi(WebServer &server, WebSocketsServer &ws, PumpManager &pumps)
    : _server(server),
      _ws(ws),
      _pumps(pumps)
{
}
void PumpApi::begin()
{
    pumpStatus();
    startPump();
    stop();
    calibrate();
    getCalibration();
}

/**
 * PUMP_STATUS
 * gives the current state of the selected pump
 * gives the current state of all pumps if no id is provided
 */
void PumpApi::pumpStatus()
{
    _server.on("/api/pump/status", HTTP_GET, [&]()
               {

    String json;

    // =========================
    // EINZELNE PUMPE
    // =========================
    if (_server.hasArg("id")) {
      uint8_t id = _server.arg("id").toInt();

      json = "{";
      json += "\"id\": " + String(id) + ",";
      json += "\"running\": ";
      json += _pumps.isRunning(id) ? "true" : "false";
      json += ",";
      json += "\"remainingMs\": ";
      json += String(_pumps.remainingMs(id));
      json += "}";

      _server.send(200, "application/json", json);
      return;
    }

    // =========================
    // ALLE PUMPEN
    // =========================
    json = "{";
    json += "\"pumps\":[";

    bool first = true;
    for (uint8_t i = 0; i < _pumps.getMaxPumps(); i++) {

      if (!first) json += ",";
      first = false;

      json += "{";
      json += "\"id\": " + String(i) + ",";
      json += "\"running\": ";
      json += _pumps.isRunning(i) ? "true" : "false";
      json += ",";
      json += "\"remainingMs\": ";
      json += String(_pumps.remainingMs(i));
      json += "}";
    }

    json += "]";
    json += "}";

    _server.send(200, "application/json", json); });
}

/**
 * START_PUMPS
 * starts one pump if id is provided
 * starts all pumps if no id is provided
 */
void PumpApi::startPump()
{
    _server.on("/api/pump/start", HTTP_POST, [&]()
               {

    if (!_server.hasArg("duration")) {
      _server.send(400, "application/json",
        "{\"error\":\"missing duration\"}");
      return;
    }

    unsigned long duration = _server.arg("duration").toInt();

    // =========================
    // EINZELNE PUMPE
    // =========================
    if (_server.hasArg("id")) {
      uint8_t id = _server.arg("id").toInt();

      if (!_pumps.start(id, duration)) {
        _server.send(404, "application/json",
          "{\"error\":\"invalid pump id\"}");
        return;
      }

      _server.send(200, "application/json",
        "{\"result\":\"started\",\"scope\":\"single\"}");
      return;
    }

    // =========================
    // ALLE PUMPEN
    // =========================
    for (uint8_t i = 0; i < _pumps.getMaxPumps(); i++) {
      _pumps.start(i, duration);
    }

    _server.send(200, "application/json",
      "{\"result\":\"started\",\"scope\":\"all\"}"); 
    broadcastMachineStatus(); });
}
/**
 * STOP
 * stops all currently running pumps
 */
void PumpApi::stop()
{
    _server.on("/api/stop", HTTP_POST, [&]()
               {
    _pumps.stopAll();
    _server.send(200, "application/json",
    "{\"result\":\"stopped\"}"); 
  broadcastMachineStatus(); });
}

void PumpApi::calibrate() {
  _server.on("/api/pump/calibrate", HTTP_POST, [&]() {

    if (!_server.hasArg("id") ||
        !_server.hasArg("ml") ||
        !_server.hasArg("seconds")) {
      _server.send(400, "application/json",
                   "{\"error\":\"missing parameters\"}");
      return;
    }

    uint8_t id = _server.arg("id").toInt();
    float ml = _server.arg("ml").toFloat();
    float sec = _server.arg("seconds").toFloat();

    if (ml <= 0 || sec <= 0) {
      _server.send(400, "application/json",
                   "{\"error\":\"invalid values\"}");
      return;
    }

    float flow = ml / sec;
    _pumps.setFlowRate(id, flow);

    _server.send(200, "application/json",
                 "{\"status\":\"calibrated\"}");
    
    broadcastPumpCalibration();
  });
  
}

void PumpApi::getCalibration() {
  _server.on("/api/pump/calibration", HTTP_GET, [&]() {

    String json = "{";
    json += "\"pumps\":[";

    bool first = true;
    for (uint8_t i = 0; i < MAX_PUMPS; i++) {

      if (!first) json += ",";
      first = false;

      json += "{";
      json += "\"id\":";
      json += String(i);
      json += ",";

      bool calibrated = _pumps.isCalibrated(i);
      json += "\"calibrated\":";
      json += calibrated ? "true" : "false";
      json += ",";

      json += "\"mlPerSec\":";
      json += String(_pumps.getFlowRate(i), 2);

      json += "}";
    }

    json += "]";
    json += "}";

    _server.send(200, "application/json", json);
  });
}

void PumpApi::broadcastPumpCalibration() {
  String json = "{";
  json += "\"type\":\"pumpCalibration\",";
  json += "\"pumps\":[";

  bool first = true;
  for (uint8_t i = 0; i < MAX_PUMPS; i++) {
    if (!first) json += ",";
    first = false;

    json += "{";
      json += "\"id\":";
      json += String(i);
      json += ",";

      bool calibrated = _pumps.isCalibrated(i);
      json += "\"calibrated\":";
      json += calibrated ? "true" : "false";
      json += ",";

      json += "\"mlPerSec\":";
      json += String(_pumps.getFlowRate(i), 2);

      json += "}";
  }

  json += "]}";

  _ws.broadcastTXT(json);
}

void PumpApi::broadcastMachineStatus()
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