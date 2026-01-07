#include "controller/pump/PumpManager.h"
#include "controller/pump/PumpController.h"

PumpManager::PumpManager()
{
  for (int i = 0; i < MAX_PUMPS; i++)
  {
    _pumps[i] = nullptr;
  }
  for (uint8_t i = 0; i < MAX_PUMPS; i++)
  {
    _flowRate[i] = 0.0f;
  }
}

bool PumpManager::addPump(uint8_t relay_PIN, bool activeLow)
{
  for (int i = 0; i < MAX_PUMPS; i++)
  {
    if (_pumps[i] == nullptr)
    {
      _pumps[i] = new PumpController(relay_PIN, activeLow);
      _pumps[i]->begin();
      return true;
    }
  }
  return false;
}

bool PumpManager::start(uint8_t id, unsigned long durationMs)
{
  if (id >= MAX_PUMPS || _pumps[id] == nullptr)
    return false;
  _pumps[id]->start(durationMs);
  return true;
}

void PumpManager::startAll(unsigned long durationMs)
{
  for (int i = 0; i < MAX_PUMPS; i++)
  {
    if (_pumps[i])
    {
      _pumps[i]->start(durationMs);
    }
  }
}

void PumpManager::stop(uint8_t id)
{
  if (id >= MAX_PUMPS || _pumps[id] == nullptr)
    return;
  _pumps[id]->stop();
}

void PumpManager::stopAll()
{
  for (int i = 0; i < MAX_PUMPS; i++)
  {
    if (_pumps[i])
    {
      _pumps[i]->stop();
    }
  }
}

bool PumpManager::isRunning(uint8_t id) const
{
  if (id >= MAX_PUMPS || _pumps[id] == nullptr)
    return false;
  return _pumps[id]->isRunning();
}

bool PumpManager::isRunning() const
{
  for (int i = 0; i < MAX_PUMPS; i++)
  {
    if (_pumps[i] && _pumps[i]->isRunning())
    {
      return true;
    }
  }
  return false;
}

unsigned long PumpManager::remainingMs() const
{
  unsigned long maxTime = 0;
  for (int i = 0; i < MAX_PUMPS; i++)
  {
    if (_pumps[i] && _pumps[i]->isRunning())
    {
      maxTime = max(_pumps[i]->remainingMs(), maxTime);
    }
  }
  return maxTime;
}

unsigned long PumpManager::remainingMs(uint8_t id) const
{
  if (id >= MAX_PUMPS || _pumps[id] == nullptr)
    return 0;
  return _pumps[id]->remainingMs();
}

void PumpManager::update()
{
  for (int i = 0; i < MAX_PUMPS; i++)
  {
    if (_pumps[i])
    {
      _pumps[i]->update();
    }
  }
}

int PumpManager::getMaxPumps()
{
  return MAX_PUMPS;
}

void PumpManager::setFlowRate(uint8_t pumpId, float mlPerSec)
{
  if (pumpId >= MAX_PUMPS || mlPerSec <= 0)
    return;
  _flowRate[pumpId] = mlPerSec;
  save();
}

float PumpManager::getFlowRate(uint8_t pumpId) const
{
  if (pumpId >= MAX_PUMPS)
    return 0.0f;
  return _flowRate[pumpId];
}

bool PumpManager::isCalibrated(uint8_t pumpId) const
{
  return getFlowRate(pumpId) > 0.0f;
}

void PumpManager::save() const
{
  Preferences prefs;
  prefs.begin("pumps", false);

  for (uint8_t i = 0; i < MAX_PUMPS; i++)
  {
    prefs.putFloat(("f" + String(i)).c_str(), _flowRate[i]);
  }

  prefs.end();
}

void PumpManager::load()
{
  Preferences prefs;
  prefs.begin("pumps", true);

  for (uint8_t i = 0; i < MAX_PUMPS; i++)
  {
    _flowRate[i] =
        prefs.getFloat(("f" + String(i)).c_str(), 0.0f);
  }

  prefs.end();
}

void PumpManager::clear()
{
  for (uint8_t i = 0; i < MAX_PUMPS; i++)
  {
    _flowRate[i] = 0.0f;
  }
  save();
}
