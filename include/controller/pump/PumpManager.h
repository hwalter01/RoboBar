#ifndef PUMP_MANAGER_H
#define PUMP_MANAGER_H

#include <Arduino.h>
#include "PumpController.h"
#include <Preferences.h>

#define MAX_PUMPS 4

class PumpManager {
  public:
    PumpManager();

    void startAll(unsigned long durationMs);
    void stopAll();
    bool start(uint8_t id, unsigned long durationMs);
    void stop(uint8_t id);

    bool addPump(uint8_t relay_PIN, bool activeLow);

    bool isRunning() const;
    unsigned long remainingMs() const;

    bool isRunning(uint8_t id) const;
    unsigned long remainingMs(uint8_t id) const;

    int getMaxPumps();

    void update();

    void setFlowRate(uint8_t id, float mlPerSec);
    float getFlowRate(uint8_t id) const;
    bool isCalibrated(uint8_t id) const;

    void load();
    void save() const;
    void clear();

  private:
    PumpController* _pumps[MAX_PUMPS];
    float _flowRate[MAX_PUMPS]; // in ml/s
};

#endif
