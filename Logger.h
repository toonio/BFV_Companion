#include <vector>
#include <Arduino.h>

class Logger {
  public:
    Logger();
    void start();
    void initSD();

  private:
    int _queueLength;
    int _samplingIntervalMS;
    std::vector<float> _speedVals;
    String _igcFolder;
    String _currentFilename;
    bool _flying;
    float _minstartSpeed;

    void updateFlying();
    void flightStarted();
    void addRecord();
};