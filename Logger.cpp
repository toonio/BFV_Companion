#include "Logger.h"
#include <SD.h>
#include <FS.h>
#include <stdio.h>
#include "Data.h"
#include <Arduino.h>

#define LILYGO_T5_V213
#include <boards.h>
SPIClass SDSPI(VSPI);


Logger::Logger()
{
  _queueLength = 100;
  _samplingIntervalMS = 1000;
  _igcFolder = "/IGC";
  _flying = false;
  _minstartSpeed = 3;
  _currentFilename = "error.txt";
}

void Logger::initSD() {
  SDSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);
  bool ok = SD.begin(SDCARD_CS, SDSPI);
  printf("Begin SDCard : %s\n", ok ? "ok" : "ko");
  printf("SD: %fG\n", SD.cardSize() / 1024.0 / 1024.0 / 1024.0);
  
  if(!SD.exists(_igcFolder))
  {
    bool ok = SD.mkdir(_igcFolder);
    printf("IGC folder created : %s\n", ok ? "ok" : "ko");
  }
}

void Logger::start() {

  while(true) {
    if(!_flying) updateFlying();
    if(_flying) {
      addRecord();
    }
    delay(1000);
  }
}

void Logger::updateFlying() {
  _speedVals.insert(_speedVals.begin(), DATA.hSpeed());
  if(_speedVals.size() > _queueLength) _speedVals.pop_back();
  bool flyingNow = true;
  int minPos = 5;
  for(int i = 0; i < minPos; i++) {
    //printf("%f, ", _speedVals[i]);
    if(_speedVals[i] < _minstartSpeed) {
      flyingNow = false;
    }
  }
  //printf("\nFlying : %d", flyingNow);

  if(!_flying && flyingNow) {
    DATA.setFlying(1);
    DATA.setFlyingStartMS(millis());
    // start of flight
    flightStarted();
  }
}

void Logger::flightStarted() {
  printf("Flight started");
  _flying = true;
  // create  new file
  char filename[128];
  sprintf(filename, "Flight-%02.0f-%02.0f-%02.0f_%02.0f-%02.0f-%02.0f.igc",DATA.year(), DATA.month(), DATA.day(), DATA.hour(), DATA.minute());
  //_currentFilename = String("flight_") + DATA.year()+ "-" +DATA.month() + "-" +DATA.day()+"-"+DATA.hour()+"-"+DATA.minute()+".igc";
  _currentFilename = filename;
  // write igc header
  File igcFile = SD.open(_igcFolder+"/"+_currentFilename, FILE_WRITE);
  if(igcFile) {
    char date[20];
    int y = DATA.year();
    y -= int(y/10)*10; //  get the 2 last digits of the year
    sprintf(date, "HFDTE%02d%02d%02d", DATA.day(), DATA.month(), y);
    igcFile.println("AXXX001BFV-Companion") ;
    igcFile.println(date) ;     // date
    igcFile.println("HFPLTPILOTINCHARGE:ASE" ) ;  // pilote
    igcFile.println("HFGTYGLIDERTYPE:HOOK5P" ) ; //glider
    igcFile.println("HFDTM100GPSDATUM:WGS-1984") ; // repere
    igcFile.println("HFFTYFRTYPE:BFV-Companion") ; // vario
    igcFile.close();
  }
}

void Logger::addRecord() {
  float latitude = DATA.latitude();
  float longitude = DATA.longitude();
  int hour = DATA.hour();
  int minute = DATA.minute();
  int second = DATA.second();
  int altitudeGPS = DATA.altitude();
  int altitudeBaro = DATA.altibaro();

  File igcFile = SD.open(_igcFolder+"/"+_currentFilename, FILE_APPEND);
  if(igcFile) {
    //igcFile.seek(EOF);

    // Convert latitude and longitude to IGC format
    char latHemisphere = (latitude >= 0) ? 'N' : 'S';
    char lonHemisphere = (longitude >= 0) ? 'E' : 'W';

    latitude = abs(latitude);
    longitude = abs(longitude);

    int latDegrees = (int)latitude;
    int latMinutes = (latitude - latDegrees) * 60.0;
    int latMinutesDec = ((latitude - latDegrees) * 60.0 - latMinutes) * 1000.0;
    int lonDegrees = (int)longitude;
    int lonMinutes = (longitude - lonDegrees) * 60.0;
    int lonMinutesDec = ((longitude - lonDegrees) * 60.0 - lonMinutes) * 1000.0;

    // Format the fix record
    char fixRecord[64];
    // B HH   MM   SS   DD   MM   MMM  N  DDD  MM   MMM  E  V  PPPPP GGGGG
    // B %02d %02d %02d %02d %02d %03d %c %03d %02d %03d %c %c %05d  %05d
    /*snprintf(fixRecord, sizeof(fixRecord), "B%02d%02d%02d%02d%05.2f%c%03d%05.2f%c%05d%05d\n",
            hour, minute, second,
            latDegrees, latMinutes, latHemisphere,
            lonDegrees, lonMinutes, lonHemisphere,
            altitudeGPS, altitudeBaro);*/

    sprintf(fixRecord, "B%02d%02d%02d%02d%02d%03d%c%03d%02d%03d%cA%05d%05d\n",
            hour, minute, second,
            latDegrees, latMinutes, latMinutesDec, latHemisphere,
            lonDegrees, lonMinutes, lonMinutesDec, lonHemisphere,
            altitudeBaro, altitudeGPS);

    // Write the fix record to the file
    igcFile.print(fixRecord);
    igcFile.close();
  }
}
