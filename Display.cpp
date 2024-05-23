#include "Display.h"
#include "Data.h"
#include "Fonts/FreeMonoBold7pt7b.h"
#include "math.h"
#include "Arduino.h"

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
  
  display.updateWindow(0, 0, display.width(), display.height());
  display.powerDown();
  //display.update();
}

void Display::drawBluetooth(bool connected) {
  display.setFont(&FreeMonoBold7pt7b);
  int x = 30;
  int y = 0;

  // draw clear box
  //display.fillRect(x, y, 10, 13, GxEPD_WHITE);
  //display.drawRect(x, y, 10, 13, GxEPD_BLACK);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds("AB", x, y, &tbx, &tby, &tbw, &tbh);
  y = tbh;

  display.fillRect(tbx, tby, tbw, tbh, GxEPD_WHITE);
  display.setCursor(x, y);
  char text[10];
  sprintf(text, "%s%s", DATA.antennaOK() ? "A" : " ", DATA.BTConnected() ? "B" : " ");
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
  display.getTextBounds("00:00 R", x, y, &tbx, &tby, &tbw, &tbh);
  display.fillRect(tbx, tby, tbw, tbh, GxEPD_WHITE);
  char text[10];
  uint32_t ms = millis() - DATA.flyingStartMS();
  int hours = ms / 1000.0 / 60.0 / 60.0;
  int minutes = (ms - (hours * 3600.0 * 1000)) / 1000.0 / 60.0;

  sprintf(text, "%02d:%02d %s", hours, minutes, DATA.flying() ? "R" : "");
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
