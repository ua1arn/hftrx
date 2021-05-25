#include <TouchScreen.h>
#include <Wire.h>

const uint8_t YP   = A1;                               // Вывод Y+ должен быть подключен к аналоговому входу
const uint8_t XM   = A0;                               // Вывод X- должен быть подключен к аналоговому входу
const uint8_t YM   = 13;                               // Вывод Y-
const uint8_t XP   = 12;                               // Вывод X+

const int tsMinX   = 250;
const int tsMinY   = 170;
const int tsMaxX   = 960;
const int tsMaxY   = 900;

TouchScreen ts     = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;

uint8_t received_command = 0;

void dataRcv(int numBytes)
{
  while(Wire.available()) {
    received_command = Wire.read();
  }
}

void dataRqst()
{
  switch (received_command) {
    case 0x01:
      Wire.write(0xBA);
      break;

    case 0x02:
      p = ts.getPoint();
      if(p.z > ts.pressureThreshhold)
      { 
        uint16_t x = constrain(map(p.x, tsMinX, tsMaxX, 0, 800), 0, 800);
        uint16_t y = constrain(map(p.y, tsMinY, tsMaxY, 0, 480), 0, 480);
 
        Wire.write(1);
        Wire.write((uint8_t) (x >> 8));
        Wire.write((uint8_t) (x & 0xFF));
        Wire.write((uint8_t) (y >> 8));
        Wire.write((uint8_t) (y & 0xFF));
        //Serial.println((String) "raw ("+p.x+","+p.y+")");
      }
      else
      {
        Wire.write(0);
        Wire.write(0xFF);
        Wire.write(0xFF);
        Wire.write(0xFF);
        Wire.write(0xFF);
      }
      break;

    default: 
      break; 
  }
}

void setup(void){ 
    Serial.begin(115200); 
    Wire.begin(8);
    Wire.onReceive(dataRcv);
    Wire.onRequest(dataRqst);
    Wire.setClock(100000);
} 
void loop(){ 
}
