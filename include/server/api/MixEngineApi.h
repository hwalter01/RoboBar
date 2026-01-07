#ifndef MIX_ENGINE_API_H
#define MIX_ENGINE_API_H

#include <WebServer.h>
#include "controller/mix/MixEngine.h"

class MixEngineApi {
public:
  MixEngineApi(WebServer& server, MixEngine& mixEngine);
  void begin();

private:
  WebServer& _server;
  MixEngine& _mixEngine;

  void start();
};

#endif
