#include "MicroNMEA.h"


class Gps {
  public:
    enum GpsStatus {
      INIT,
      BUSY,
      READY
    };

    Gps();
    void start();
    GpsStatus getStatus();

  private:
    GpsStatus _status;

    static void badChecksumHandler(MicroNMEA& nmea);
    static void unknownSentenceHandler(MicroNMEA& nmea);
};