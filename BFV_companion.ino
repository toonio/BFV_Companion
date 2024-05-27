#include "BluetoothSerial.h"
#include <string>
#include "Vario.h"
#include "Display.h"
#include "Gps.h"
#include "Data.h"
#include "Logger.h"
#include "math.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif
 
// Bluetooth Serial Object (Handle)
//BluetoothSerial SerialBT;
Vario vario;
Display display;
Gps gps;
Logger logger;
//uint8_t BFVAddress[6] = {0x00, 0x06, 0x66, 0x74, 0x42, 0xec};

void printBTDiscoveryResults(BTScanResults *res) {
  char buff[256];
  int nbDevices = res->getCount();
  sprintf(buff, "Found %d devices !\n", nbDevices);
  Serial.write(buff);
  for(int i=0; i < nbDevices; i++) {
    sprintf(buff, "Device #%d\n", i);
    Serial.write(buff);
    sprintf(buff, "%s @ %s\n", res->getDevice(i)->getName().c_str(), res->getDevice(i)->getAddress().toString().c_str());
    Serial.write(buff);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.write("BlueFly Vario Companion V0.0 - Hello up there !\n");
  delay(200);

  display.init();
  display.showWelcome();

  printf("Coucou\n");

  // start tasks
  xTaskCreate(taskVario, "taskVario", 4080, NULL, 1, NULL);
  xTaskCreate(taskDisplay, "taskDisplay", 4080, NULL, 1, NULL);
  xTaskCreate(taskGPS, "taskGPS", 4080, NULL, 1, NULL);
  xTaskCreate(taskLogger, "taskLogger", 4080, NULL, 1, NULL);
}

void taskLogger( void * pvParameters )
 {
    logger.initSD();
    logger.start();
 }

void taskVario( void * pvParameters )
 {
    vario.init();
    if(vario.status() == Vario::CONNECTED) {
      vario.start();
    }

    while(true){
      delay(200);
    }

    /*int i = 0;
    while(true) {
      delay(200);
      DATA.setVSpeed(fmod((i++ / 10.0), 8.0) - 2.0);
    }*/
 }

 void taskDisplay( void * pvParameters )
 {
    delay(3000);
    display.clear();
    while(true) {
      display.update();
      delay(1);
    }
 }

 void taskGPS( void * pvParameters ) {
  gps.start();
 }

void loop() {
  
  delay(200);
}
