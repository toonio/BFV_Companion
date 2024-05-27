#include "Data.h"

#define POS_QUEUE_SIZE 1000

Data * Data::_instance = nullptr;

Data::Data() {
  _mutex = xSemaphoreCreateMutex();
  _posmutex = xSemaphoreCreateMutex();
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
  _altibaro = 0;
  _BTConnected = 0;
  _nbGPSFix = 0;
  _deltaGPS = 0;
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

void Data::pushLat(float value) {
  if(xSemaphoreTake(_posmutex, 10)) {
    _lats.insert(_lats.begin(), value);
    if(_lats.size() > POS_QUEUE_SIZE) _lats.pop_back();
    xSemaphoreGive(_posmutex);
  }
}

void Data::pushLon(float value) {
  if(xSemaphoreTake(_posmutex, 10)) {
    _lons.insert(_lons.begin(), value);
    if(_lons.size() > POS_QUEUE_SIZE) _lons.pop_back();
    xSemaphoreGive(_posmutex);
  }
}

std::vector<float> Data::getLats() {
  if(xSemaphoreTake(_posmutex, 10)) {
    std::vector<float> cpy = _lats;
    xSemaphoreGive(_posmutex);
    return cpy;
  }
  return std::vector<float>();
}

std::vector<float> Data::getLons() {
  if(xSemaphoreTake(_posmutex, 10)) {
    std::vector<float> cpy = _lons;
    xSemaphoreGive(_posmutex);
    return cpy;
  }
  return std::vector<float>();
}
