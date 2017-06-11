/*
 * Sun Microsystems Mouse signal receiver
 * Tested with Arduino Duemilanove
 * 
 * Referred document : http://www.rosenau-ka.de/ps2sun/
 * 
 * Pin connection
 *  +5V --- Arduino 5V
 *  GND --- Arduino GND
 *  Mouse data --- Arduino Digital pin 2
 */
#include <SoftwareSerial.h>

SoftwareSerial swSer(2, 3, true);
bool button_l;
bool button_m;
bool button_r;
int32_t pos_x;
int32_t pos_y;

void setup() {
  Serial.begin(115200);
  swSer.begin(1200);
  Serial.println("Sun Microsystems mouse receiver");
}

bool checkDataValid(byte* data)
{
  if ((*data & 0b11111000) == 0b10000000) {
    return true;
  }
  return false;
}

void parseData(byte* data)
{
  int8_t x;
  int8_t y;

  button_l = data[0] & 4 ? true : false;
  button_m = data[0] & 2 ? true : false;
  button_r = data[0] & 1 ? true : false;

  x = data[1] + data[3];
  y = data[2] + data[4];

  pos_x += x;
  pos_y += y;
}

static char str[64];
void showData(void)
{
  sprintf(str, "[%c][%c][%c] ",
          button_l ? ' ' : '*',
          button_m ? ' ' : '*',
          button_r ? ' ' : '*'
         );
  Serial.print(str);
  Serial.print("("); Serial.print(pos_x);Serial.print(",");Serial.print(pos_y);Serial.println(")");
}

void loop() {
  int i;
  byte readBuff[5];
  volatile byte tmp;

  while (swSer.available() >= 5) {
    for (i = 0; i < 5; i++) readBuff[i] = swSer.read();
    if (checkDataValid(readBuff) == true) {
      parseData(readBuff);
      showData();
    } else {
      Serial.println("skipped");
      while(swSer.available() > 0){
        tmp = swSer.read();
      }
    }
  }
}
