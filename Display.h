#define LILYGO_T5_V213
#include <boards.h>
#include <GxEPD.h>
#include <SD.h>
#include <FS.h>
#include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

//GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
//GxEPD_Class display(io, EPD_RSET, EPD_BUSY);  

class Display {
  public:
    Display();
    void init();
    void showWelcome();
    void clear();
    void update();

  private:
    GxIO_Class io;
    GxEPD_Class display;

    void drawBluetooth(bool connected);
    void drawVarioBar(float value);
    void drawVarioVal(float value);
    void drawSpeedVal(float value);
    void drawAltitudeVal(float value);
    void drawHeadingVal(float value);
    void drawTime();
    void drawUptime();
};