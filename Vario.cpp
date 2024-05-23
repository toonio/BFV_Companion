#include <string>
#include "Vario.h"
#include "Data.h"
#include <math.h>
#include <Vector>

uint8_t BFVAddress[6] = {0x00, 0x06, 0x66, 0x74, 0x42, 0xec};
#define BT_TIMEOUT 3000

Vario::Vario(): _status(VarioStatus::CONNECTING), _buffer("")
{
  _lastMsgTime = 0;
}

void Vario::init() {
  _serialBT.begin("BFV Companion", true);
  Serial.println("Connecting... ");
  bool connected = _serialBT.connect(BFVAddress);
  int retry = 0;
  while(!connected && retry++ < 10) {
    Serial.print("Vario retry\n");
    connected = _serialBT.connect(BFVAddress);
  }
  Serial.print("Vario connection : ");
  if(connected) {
    Serial.print("OK ! :)\n");
    _status = VarioStatus::CONNECTED;
    // set vario output mode LK8EX1
    auto cmd = "$BOM 1*"; 
    _serialBT.write((uint8_t*)cmd, sizeof(cmd));
  }
  else {
    Serial.print("KO ! :(\n");
    _status = VarioStatus::FAILED;
  }
}

void Vario::start() {
  _running = true;
  while(_running && _status == CONNECTED) {
    if(_serialBT.available()) {
      const char c = _serialBT.read();
      if(c == '$') {
        _buffer = c;
      } else if(c == '\n') {
        _buffer.push_back(c);
        //Serial.write(_buffer.c_str());
        processMessageLK8EX1(_buffer);
        _buffer.clear();
      } else {
        _buffer.push_back(c);
      }
    }
    if(millis() - _lastMsgTime > BT_TIMEOUT) {
      DATA.setBTConnected(0);
    }
  }
}

void Vario::stop() {
  _running = false;
}

void Vario::processMessageLK8EX1(const std::string &msg) {
  //Serial.write(msg.substr(0, 7).c_str());
  if(msg.substr(0, 7) == "$LK8EX1") {
    _lastMsgTime = millis();
    auto data = split(msg, ',');
    // data = $LK8EX1,pressure,altitude,vario,temperature,battery,*checksum\r\n"
    /*Serial.write("Vario : ");
    Serial.write(data[3].c_str());
    Serial.write("cm/s\n");  */

    DATA.setAltibaro(pressToAlti(std::stoi(data[1])/100.0)); // Pa to HPa (mbar)
    DATA.setVSpeed(std::stoi(data[3])/100.0);
    DATA.setVarioBattery(std::stof(data[5]));
    DATA.setBTConnected(1);
  }
}

float Vario::pressToAlti(float pressMb) {
  float alti = 145366.45 * (1-pow(pressMb/1013.25, 0.190284));
  return alti;
}

std::vector<std::string> Vario::split(const std::string& s, char seperator)
{
   std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word

    return output;
}
