#include "Display.h"
#include "Data.h"
#include "Fonts/FreeMonoBold7pt7b.h"
#include "math.h"
#include "Arduino.h"
#include <vector>
#include <algorithm>

Display::Display():io(SPI,  EPD_CS, EPD_DC,  EPD_RSET), display(io, EPD_RSET, EPD_BUSY)
{
}

void Display::init() {
  SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
  display.init();
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.updateWindow(0, 0, display.width(), display.height());
  std::string msg = std::to_string(display.width()) + "x" + std::to_string(display.height()) + "\n";
  Serial.write(msg.c_str());
}

void Display::showWelcome() {
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(0, 0);
  display.println();
  display.println("Coucou\nToonio !<3");
  display.update();
}

void Display::clear() {
  display.fillScreen(GxEPD_WHITE);
  display.update();
}

void Display::update() {
  static int cpt(0);
  display.init();

  if(fmod(cpt++, 10.0) == 0) {
    display.fillScreen(GxEPD_WHITE);
    display.updateWindow(0, 0, display.width(), display.height());
  }
  drawBluetooth(true);
  drawTime();
  float vario = DATA.vSpeed();
  float speed = DATA.hSpeed();
  drawVarioBar(vario);
  drawVarioVal(vario);
  drawSpeedVal(speed);
  drawAltitudeVal(DATA.altitude());
  drawHeadingVal(DATA.heading());
  drawUptime();
  drawMap();
  
  display.updateWindow(0, 0, display.width(), display.height());
  display.powerDown();
  //display.update();
}

void Display::drawBluetooth(bool connected) {
  display.setFont(&FreeMonoBold7pt7b);
  int x = 20;
  int y = 0;

  // draw clear box
  //display.fillRect(x, y, 10, 13, GxEPD_WHITE);
  //display.drawRect(x, y, 10, 13, GxEPD_BLACK);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds("ABR", x, y, &tbx, &tby, &tbw, &tbh);
  y = tbh;

  display.fillRect(tbx, tby, tbw, tbh, GxEPD_WHITE);
  display.setCursor(x, y);
  char text[10];
  sprintf(text, "%s%s%s", DATA.antennaOK() ? "A" : " ", DATA.BTConnected() ? "B" : " ", DATA.flying() ? "R" : "");
  display.print(text);
}

void Display::drawTime() {
  static bool even(false);
  display.setFont(&FreeMonoBold7pt7b);
  int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
  display.getTextBounds("00:00", 0, 0, &tbx, &tby, &tbw, &tbh);
  int x = display.width() - tbw - 5;
  int y = tbh;
  display.getTextBounds("00:00", x, y, &tbx, &tby, &tbw, &tbh);
  display.fillRect(tbx, tby, tbw, tbh, GxEPD_WHITE);
  char text[10];
  sprintf(text, even ? "%02.0f %02.0f" : "%02.0f:%02.0f", DATA.hour()+2, DATA.minute()); // warning GMT time
  display.setCursor(x, y);
  display.print(text);
  even = !even;
}

void Display::drawUptime() {
  display.setFont(&FreeMonoBold7pt7b);
  int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
  int x = 20;
  int y = 245;
  display.getTextBounds("00:00", x, y, &tbx, &tby, &tbw, &tbh);
  display.fillRect(tbx, tby, tbw, tbh, GxEPD_WHITE);
  char text[10];
  uint32_t ms = millis() - DATA.flyingStartMS();
  int hours = ms / 1000.0 / 60.0 / 60.0;
  int minutes = (ms - (hours * 3600.0 * 1000)) / 1000.0 / 60.0;

  sprintf(text, "%02d:%02d", hours, minutes);
  display.setCursor(x, y);
  display.print(text);
}

void Display::drawVarioBar(float value) {
  // draw clear box
  display.fillRect(0, 0, 12, 250, GxEPD_WHITE);
  display.drawRect(0, 0, 12, 125, GxEPD_BLACK);
  display.drawRect(0, 125, 12, 125, GxEPD_BLACK);

  // draw value bar
  int maxVal = 6; // m/s
  int height = value * 125.0 / maxVal * -1;
  display.fillRect(0, 125, 12, height, GxEPD_BLACK);
  if(value < 0) {
    display.fillRect(4, 125, 8, height-2, GxEPD_WHITE);
  }
}

void Display::drawVarioVal(float value) {
  int x = 30;
  int y = 60;
  // unit
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(x+10,y+20);
  display.print("m/s");

  // value
  display.setFont(&FreeMonoBold18pt7b);
  char text[10];
  sprintf(text, "%.1f", fabs(value));
  int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
  display.getTextBounds("0.0", x, y, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x, y); // set the postition to start printing text
  //printf("%d %d %d %d %s\n", tbx, tby+tbh, tbw, tbh, text);
  display.fillRect(tbx-4, tby-4, tbw+8, tbh+8, value >= 0 ? GxEPD_WHITE : GxEPD_BLACK);
  display.setTextColor(value >= 0 ? GxEPD_BLACK : GxEPD_WHITE);
  display.print(text); // print some text
}

void Display::drawSpeedVal(float value) {
  int x = 30;
  int y = 120;
  // unit
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(x+10,y+20);
  display.print("km/h");

  // value
  display.setFont(&FreeMonoBold18pt7b);
  char text[10];
  sprintf(text, "%2.0f", fabs(value));
  //printf("spd: %.0f", fabs(value));
  int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
  display.getTextBounds("00", x, y, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x, y); // set the postition to start printing text
  //printf("%d %d %d %d %s\n", tbx, tby+tbh, tbw, tbh, text);
  display.fillRect(tbx-4, tby-4, tbw+8, tbh+8, value >= 0 ? GxEPD_WHITE : GxEPD_BLACK);
  display.setTextColor(value >= 0 ? GxEPD_BLACK : GxEPD_WHITE);
  display.print(text); // print some text
}

void Display::drawAltitudeVal(float value) {
  int x = 20;
  int y = 180;
  
  display.setFont(&FreeMonoBold7pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(x,y);
  display.print("Alt(m)");

  display.setFont(&FreeMonoBold9pt7b);
  int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
  display.getTextBounds("0000", x, y+20, &tbx, &tby, &tbw, &tbh);
  display.fillRect(tbx-4, tby-4, tbw+8, tbh+8, GxEPD_WHITE);
  char text[10];
  sprintf(text, "%04.0f", fabs(value));
  display.setCursor(x,y+20);
  display.print(text);
}

void Display::drawHeadingVal(float value) {

}

void Display::drawMap() {
  int x = 65;
  int y = 190;
  int w = 55;
  int h = 55;

  display.fillRect(x, y, w, h, GxEPD_WHITE);
  display.drawRect(x, y, w, h, GxEPD_BLACK);

  std::vector<float> lats = DATA.getLats();
  std::vector<float> lons = DATA.getLons();

  if(lats.size() > 1) {
    float minLat = *std::min_element(begin(lats), end(lats));
    float maxLat = *std::max_element(begin(lats), end(lats));
    float minLon = *std::min_element(begin(lons), end(lons));
    float maxLon = *std::max_element(begin(lons), end(lons));
    
    float speed = max(DATA.hSpeed(), 5.0f);
    double offset = (speed * 5) / 111320.0; //m to dega
    minLat = lats[0] - offset;
    maxLat = lats[0] + offset;
    minLon = lons[0] - (offset / cos(lats[0] * 0.01745));
    maxLon = lons[0] + (offset / cos(lats[0] * 0.01745));
    float delta = max(maxLat-minLat, maxLon-minLon);

    if(maxLat - minLat != 0) {
      //printf("#pos : %d\n", lats.size());
      for(int i = 0; i < lats.size()-1; i++) {
        float lat = max(min(lats[i], maxLat), minLat); //lats[i]; //
        float lon = max(min(lons[i], maxLon), minLon); //lons[i]; //
        int yPt = h - ((lat - minLat) / (maxLat-minLat) * h);
        int xPt = (lon - minLon) / (maxLon-minLon) * w;
        lat = max(min(lats[i+1], maxLat), minLat); //lats[i+1];//
        lon = max(min(lons[i+1], maxLon), minLon); //lons[i+1];//
        int yPt2 = h - ((lat - minLat) / (maxLat-minLat) * h);
        int xPt2 = (lon - minLon) / (maxLon-minLon) * w;
        if(i == 0) { // most current pos
          display.fillCircle(x + xPt, y + yPt, 3, GxEPD_BLACK);
        }
        //display.drawPixel(x + xPt, y + yPt, GxEPD_BLACK);
        display.drawLine(x + xPt, y + yPt, x + xPt2, y + yPt2, GxEPD_BLACK);
        //printf("(%d, %d) (%d, %d)\n)", x + xPt, y + yPt, x + xPt2, y + yPt2 );
      }
    }
    else {
      display.drawPixel(x+(w/2), y+(h/2), GxEPD_BLACK);
    }
  }
}
