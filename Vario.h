#include "BluetoothSerial.h"
#include <Vector>

class Vario {
  public:

    enum VarioStatus {
      CONNECTING,
      CONNECTED,
      FAILED
    };

    Vario();
    void init();
    void start();
    void stop();
    VarioStatus status() { return _status; };

  private:
    bool _running;
    std::string _buffer;
    BluetoothSerial _serialBT;
    VarioStatus _status;
    long _lastMsgTime;

    void processMessageLK8EX1(const std::string &msg);
    std::vector<std::string> split(const std::string& s, char seperator);
    float pressToAlti(float press);
};