#include "Data.h"

Data * Data::_instance = nullptr;

Data::Data() {
  _mutex = xSemaphoreCreateMutex();
  _battery = 0;
  _varioBattery = 0;
  _vSpeed = 0;
  _hSpeed = 0;
  _heading = 0;
  _flightStartTime = 0;
  _altitude = 0;
  _hour = 0;
  _minute = 0;
  _latitude = 0;
  _longitude = 0;
  _day = 0;
  _month = 0;
  _year = 0;
  _flying = 0;
  _flyingStartMS = 0;
  _antennaOK = 0;
  _BTConnected = 0;
}

Data & Data::instance() {
  if(!_instance)
    _instance = new Data();

  return *_instance;
}


float Data::getWithMutex(float *data) {
  float v = NO_VALUE;
  if(xSemaphoreTake(_mutex, 10)) {
    v = *data;
    xSemaphoreGive(_mutex);
  }
  return v;
}

void Data::setWithMutex(float *data, float value) {
  if(xSemaphoreTake(_mutex, 10)) {
    *data = value;
    xSemaphoreGive(_mutex);
  }
}

int Data::getWithMutex(int *data) {
  int v = NO_VALUE;
  if(xSemaphoreTake(_mutex, 10)) {
    v = *data;
    xSemaphoreGive(_mutex);
  }
  return v;
}

void Data::setWithMutex(int *data, int value) {
  if(xSemaphoreTake(_mutex, 10)) {
    *data = value;
    xSemaphoreGive(_mutex);
  }
}