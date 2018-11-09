/*
 * Nucleo DCO I2C Slave / OLED Module Test.
 *
 * 2018.11.07
 *
 */

#include <Wire.h>
#include <U8g2lib.h>

#define TITLE_STR1  ("I2C Slave Test")
#define TITLE_STR2  ("20181109")

#define I2C_ADDR   (0x08)
#define I2C_CLOCK  (100000)

// display mode
enum {
  DM_TITLE       = 0,
  DM_NORMAL      = 1,
  DM_FREQUENCY   = 2,
  DM_AMPLITUDE   = 3,
  DM_PULSE_WIDTH = 4,
  DM_DISPLAY_OFF = 255
};

//U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

void setup()
{
  Serial.begin(9600);           // start serial for output
  Serial.println();
  Serial.println(TITLE_STR1);
  Serial.println(TITLE_STR2);
  
  // OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_8x13_tf);
  do {
    u8g2.drawStr(0,10,TITLE_STR1);
    u8g2.drawStr(0,25,TITLE_STR2);
  } while (u8g2.nextPage());
  u8g2.sendBuffer();
  
  // I2C
  Wire.begin(I2C_ADDR);         // join i2c bus with address #8
  pinMode(A4, INPUT);           // disable pullup
  pinMode(A5, INPUT);           // disable pullup
  Wire.setClock(I2C_CLOCK);
  Wire.onReceive(receiveEvent); // register event

  delay(2000);
}

void loop()
{
  delay(100);
}

void receiveEvent(int byteN)
{
  Serial.println("receiveEvent()");
  
  int mode = Wire.read();
  Serial.print("mode: ");
  Serial.println(mode);

  char titleStr1[16];
  char titleStr2[16];
  char titleStr3[16];
  
  for (int i = 0; i < 16; i++) {
    titleStr1[i] = Wire.read();
  }
  
  for (int i = 0; i < 16; i++) {
    titleStr2[i] = Wire.read();
  }
  /*
  for (int i = 0; i < 16; i++) {
    titleStr3[i] = Wire.read();
  }
  */  
  Serial.println(String(titleStr1));
  Serial.println(String(titleStr2));
  //Serial.println(String(titleStr3));
  
/*
    CheckPin1.write(1);
    switch (displayMode) {
    case DM_TITLE:
      displayTitle();
      break;
    case DM_NORMAL:
      displayNormal();
      break;
    case DM_FREQUENCY:
      displayFrequency();
      break;
    case DM_AMPLITUDE:
      displayAmplitude();
      break;
    case DM_PULSE_WIDTH:
      displayPulseWidth();
      break;
    case DM_DISPLAY_OFF:
      displayOff();
      break;
    }
    CheckPin1.write(0);
*/
/*  
  char buff1[20];
  char buff2[30];
  int i = 0;
  
  while (1 < Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
    if (i < 18) {
      buff1[i] = c;
      i++;
    }
    buff1[i] = '\0';
  }
  int x = Wire.read();
  Serial.println(x);
  sprintf(buff2, "%s%d", buff1, x);
  
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_10x20_mf);
  do {
    u8g2.drawStr(0,32, buff2);
  } while (u8g2.nextPage());
*/
}

