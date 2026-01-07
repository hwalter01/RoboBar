#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>

class PumpController {
  public:
    PumpController(uint8_t pin, bool activeLow = true);

    void begin();
    void start(unsigned long durationMs);
    void stop();
    void update();

    bool isRunning() const;

    unsigned long remainingMs() const;

  private:
    uint8_t _pin;
    bool _activeLow;

    unsigned long _startTime;
    
    unsigned long _duration;
    bool _running;

    void relayOn();
    void relayOff();
};

#endif
