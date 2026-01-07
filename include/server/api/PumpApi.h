#ifndef PUMP_API_H
#define PUMP_API_H

#include <WebServer.h>
#include <WebSocketsServer.h>

#include "controller/pump/PumpManager.h"

class PumpApi {
  public:
    PumpApi(WebServer& server, WebSocketsServer& ws, PumpManager& pumps);
    void begin();

  private:
    WebServer& _server;
    WebSocketsServer& _ws;
    PumpManager& _pumps;

    void pumpStatus();
    void startPump();
    void stop();
    void calibrate();
    void getCalibration();
    void broadcastPumpCalibration();
    void broadcastMachineStatus();
};

#endif