/*
   Sun Microsystems Mouse signal receiver
   Tested with Arduino Leonardo

   Yasuhiro ISHII ishii.yasuhiro@gmail.com
   Twitter : @yishii

   Referred document : http://www.rosenau-ka.de/ps2sun/

   Pin connection
    +5V        --- Arduino 5V
    GND        --- Arduino GND
    Mouse data --- Arduino Digital pin 8
*/
#include <SoftwareSerial.h>
#include "Mouse.h"

SoftwareSerial swSer(8, 2, true);
bool button_l;
bool button_m;
bool button_r;
int16_t move_x;
int16_t move_y;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(115200);
  while (!Serial);
  swSer.begin(1200);
  Serial.println("Sun Microsystems mouse converter");
  Mouse.begin();
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

  move_x = data[1] + data[3];
  move_y = data[2] + data[4];

}


void updateUsbMouseEvent(void)
{
  Mouse.move(move_x, -move_y);
  if (button_l) {
    Mouse.release(MOUSE_LEFT);
    digitalWrite(13, LOW);
  } else {
    Mouse.press(MOUSE_LEFT);
    digitalWrite(13, HIGH);
  }
  if (button_m) {
    Mouse.release(MOUSE_MIDDLE);
  } else {
    Mouse.press(MOUSE_MIDDLE);
  }
  if (button_r) {
    Mouse.release(MOUSE_RIGHT);
  } else {
    Mouse.press(MOUSE_RIGHT);
  }
}

void loop() {
  int i;
  byte readBuff[5];
  volatile byte tmp;

  if (swSer.available() >= 5) {
    for (i = 0; i < 5; i++) readBuff[i] = swSer.read();
    if (checkDataValid(readBuff) == true) {
      parseData(readBuff);
      updateUsbMouseEvent();
    } else {
      Serial.println("skipped");
      while (swSer.available() > 0) {
        tmp = swSer.read();
      }
    }
  }
}


