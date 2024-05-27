#include "Gps.h"
#include "Data.h"
#include "math.h"
#include <Arduino.h>

//#define SIMU

#ifdef SIMU
#include "nmea_dump.h"
#endif

char buff[128];
MicroNMEA _nmea(buff, sizeof(buff));
  

Gps::Gps() : _status(INIT) {
  //_nmea.setBadChecksumHandler(badChecksumHandler);
  _nmea.setUnknownSentenceHandler(unknownSentenceHandler);
}

void Gps::start() {
  printf("GPS Begin serial\n");
  Serial1.begin(9600, SERIAL_8N1, 33, 32);
  char c;
  long alt(0), spd(0), hdg(0), lat(0), lon(0);
  while(true) {
    delay(1);
#ifndef SIMU
    if(Serial1.available()) {
      c = Serial1.read();
#else
    static int j = 0;
    c = nmea_dump[j++];
    delay(5);
    if(strcmp(&c, "~")==0) {
      j = 0;
      continue;
    }
    {
#endif
      //printf("%c", c);
      if (_nmea.process(c)) {
        if(_nmea.isValid()) {
          //printf("valid sentence\n");
          _status = READY;
          _nmea.getAltitude(alt);
          spd = _nmea.getSpeed();
          hdg = _nmea.getCourse();
          lat = _nmea.getLatitude();
          lon = _nmea.getLongitude();
          float altitude = alt / 1000.0;
          float speed = spd / 1000.0 * 1.852;
          float heading = hdg / 1000.0;

          //printf("Alt : %f, Spd: %f, Hdg: %f, L,l:%ld %ld\n", altitude, speed, heading, lat, lon);
          DATA.setAltitude(altitude);
          DATA.setHSpeed(speed);
          DATA.setHeading(heading);
          DATA.setHour(_nmea.getHour());
          DATA.setMinute(_nmea.getMinute());
          DATA.setSecond(_nmea.getSecond());
          DATA.setLatitude(lat/1000000.0);
          DATA.setLongitude(lon/1000000.0);
          DATA.setDay(_nmea.getDay());
          DATA.setMonth(_nmea.getMonth());
          DATA.setYear(_nmea.getYear());
        }
      }
    }
  }
  int i = 0;
  while(true) {
    delay(200);
    DATA.setHSpeed(fmod((i++ / 3.0), 27.0) - 5.0);
  }
}

Gps::GpsStatus Gps::getStatus() {
  return Gps::INIT;
}

void Gps::badChecksumHandler(MicroNMEA& nmea) {
  printf("Bad Chksm : %s", nmea.getSentence());  
}

void Gps::unknownSentenceHandler(MicroNMEA& nmea) {
  //printf("Unknown sentence : %s\n", nmea.getSentence());
  String s = nmea.getSentence();
  if(s.startsWith("$GPTXT")) {
    if(s.indexOf("ANTENNA OK") >= 0) {
      DATA.setAntennaOK(1);
    }
    else if(s.indexOf("ANTENNA OPEN") >=0 ) {
      DATA.setAntennaOK(0);
    }
  }
}
