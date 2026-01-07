#include "controller/pump/PumpController.h"

PumpController::PumpController(uint8_t pin, bool activeLow)
: _pin(pin),
  _activeLow(activeLow),
  _startTime(0),
  _duration(0),
  _running(false) {}

void PumpController::begin() {
  pinMode(_pin, OUTPUT);
  stop(); // sicherer Start
}

void PumpController::start(unsigned long durationMs) {
  _duration = durationMs;
  _startTime = millis();
  _running = true;
  relayOn();
}

void PumpController::stop() {
  relayOff();
  _running = false;
}

void PumpController::update() {
  if (!_running) return;

  if (millis() - _startTime >= _duration) {
    stop();
  }
}

bool PumpController::isRunning() const {
  return _running;
}

void PumpController::relayOn() {
  digitalWrite(_pin, _activeLow ? LOW : HIGH);
}

void PumpController::relayOff() {
  digitalWrite(_pin, _activeLow ? HIGH : LOW);
}

unsigned long PumpController::remainingMs() const {
  if (!_running) return 0;
  unsigned long elapsed = millis() - _startTime;
  return (elapsed >= _duration) ? 0 : (_duration - elapsed);
}
