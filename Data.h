#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


#define DATA Data::instance()

#define NO_VALUE -9999

class Data {
  public:
    static Data & instance();

    float battery() { return getWithMutex(&_battery); }
    void setBattery(float x) { setWithMutex(&_battery, x); }

    float varioBattery() { return getWithMutex(&_varioBattery); }
    void setVarioBattery(float x) { setWithMutex(&_varioBattery, x); }

    float vSpeed() { return getWithMutex(&_vSpeed); }
    void setVSpeed(float x) { setWithMutex(&_vSpeed, x); }

    float hSpeed() { return getWithMutex(&_hSpeed); }
    void setHSpeed(float x) { setWithMutex(&_hSpeed, x); }

    float heading() { return getWithMutex(&_heading); }
    void setHeading(float x) { setWithMutex(&_heading, x); }

    float flightStartTime() { return getWithMutex(&_flightStartTime); }
    void setFlightStartTime(float x) { setWithMutex(&_flightStartTime, x); }

    float altitude() { return getWithMutex(&_altitude); }
    void setAltitude(float x) { setWithMutex(&_altitude, x); }

    float altibaro() { return getWithMutex(&_altibaro); }
    void setAltibaro(float x) { setWithMutex(&_altibaro, x); }
    
    float hour() { return getWithMutex(&_hour); }
    void setHour(float x) { setWithMutex(&_hour, x); }

    float minute() { return getWithMutex(&_minute); }
    void setMinute(float x) { setWithMutex(&_minute, x); }

    float second() { return getWithMutex(&_second); }
    void setSecond(float x) { setWithMutex(&_second, x); }

    float latitude() { return getWithMutex(&_latitude); }
    void setLatitude(float x) { setWithMutex(&_latitude, x); }

    float longitude() { return getWithMutex(&_longitude); }
    void setLongitude(float x) { setWithMutex(&_longitude, x); }

    int day() { return getWithMutex(&_day); }
    void setDay(int x) { setWithMutex(&_day, x); }
    
    int month() { return getWithMutex(&_month); }
    void setMonth(int x) { setWithMutex(&_month, x); }
  
    int year() { return getWithMutex(&_year); }
    void setYear(int x) { setWithMutex(&_year, x); }

    int flying() { return getWithMutex(&_flying); }
    void setFlying(int x) { setWithMutex(&_flying, x); }

    int flyingStartMS() { return getWithMutex(&_flyingStartMS); }
    void setFlyingStartMS(int x) { setWithMutex(&_flyingStartMS, x); }

    int antennaOK() { return getWithMutex(&_antennaOK); }
    void setAntennaOK(int x) { setWithMutex(&_antennaOK, x); }

    int BTConnected() { return getWithMutex(&_BTConnected); }
    void setBTConnected(int x) { setWithMutex(&_BTConnected, x); }

  
  private:
    float _battery;
    float _varioBattery;
    float _vSpeed;
    float _hSpeed;
    float _heading;
    float _flightStartTime;
    float _altitude;
    float _altibaro;
    float _hour;
    float _minute;
    float _second;
    float _latitude;
    float _longitude;
    int _day;
    int _month;
    int _year;
    int _flying;
    int _flyingStartMS;
    int _antennaOK;
    int _BTConnected;
    
    Data();
    static Data * _instance;

    float getWithMutex(float *data);
    void setWithMutex(float *data, float value);
    int getWithMutex(int *data);
    void setWithMutex(int *data, int value);

    SemaphoreHandle_t _mutex;
    
};