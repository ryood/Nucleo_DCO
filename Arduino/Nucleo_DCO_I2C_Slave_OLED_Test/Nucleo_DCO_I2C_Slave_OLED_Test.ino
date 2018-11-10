/*
 * Nucleo DCO I2C Slave / OLED Module Test.
 *
 * 2018.11.07
 *
 */

#include <Wire.h>
#include <U8g2lib.h>

#define TITLE_STR1  ("I2C Slave Test")
#define TITLE_STR2  ("20181111")

#define I2C_ADDR   (0x08)
#define I2C_CLOCK  (100000)

#define MASTER_TITLE_STR_LEN (32)
#define OSC_NUM    (3)

// display mode
enum {
  DM_TITLE       = 0,
  DM_NORMAL      = 1,
  DM_FREQUENCY   = 2,
  DM_AMPLITUDE   = 3,
  DM_PULSE_WIDTH = 4,
  DM_TITLE_STR1  = 128,
  DM_TITLE_STR2  = 129,
  DM_TITLE_STR3  = 130,
  DM_DISPLAY_OFF = 255
};

//U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

const int CheckPin1 = 2;

char masterTitleStr1[MASTER_TITLE_STR_LEN] = "TitleStr1";
char masterTitleStr2[MASTER_TITLE_STR_LEN] = "TitleStr2";
char masterTitleStr3[MASTER_TITLE_STR_LEN] = "TitleStr3";

volatile int displayMode = DM_TITLE;

volatile int waveShape[OSC_NUM];
volatile int frequencyRange[OSC_NUM];
volatile int fps;
volatile int batteryVoltage;
volatile bool adcAvailable;

volatile uint8_t fpsh;
volatile uint8_t fpsl;

void setup()
{
  Serial.begin(9600);           // start serial for output
  Serial.println();
  Serial.println(TITLE_STR1);
  Serial.println(TITLE_STR2);

  pinMode(CheckPin1, OUTPUT);
  
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
  switch (displayMode) {
  case DM_TITLE:
    Serial.println("******* Display Title *********");
    Serial.println(masterTitleStr1);
    Serial.println(masterTitleStr2);
    Serial.println(masterTitleStr3);
    Serial.println("*******************************");
    break;
  case DM_NORMAL:
    Serial.println("*********** Normal ************");
    Serial.print("WaveShpe1: ");   Serial.println(waveShape[0]);
    Serial.print("WaveShpe2: ");   Serial.println(waveShape[1]);
    Serial.print("WaveShpe3: ");   Serial.println(waveShape[2]);
    Serial.print("FreqRange1: ");  Serial.println(frequencyRange[0]);
    Serial.print("FreqRange2: ");  Serial.println(frequencyRange[1]);
    Serial.print("FreqRange3: ");  Serial.println(frequencyRange[2]);
    Serial.print("Fps(H): ");      Serial.println(fpsh); 
    Serial.print("Fps(L): ");      Serial.println(fpsl); 
    Serial.print("Fps: ");         Serial.println(fps);
    Serial.print("Fps: ");         Serial.print(fps/10); Serial.print("."); Serial.println(fps%10); 
    Serial.print("BattVoltage: "); Serial.println(batteryVoltage);
    Serial.print("BattVoltage: "); Serial.print(batteryVoltage/10); Serial.print("."); Serial.println(batteryVoltage%10);
    Serial.print("ADCAvilable: "); Serial.println(adcAvailable);
    Serial.println("*******************************");
  }
  delay(100);
}

void receiveEvent(int byteN)
{
  Serial.println("receiveEvent()");
  
  displayMode = Wire.read();
  Serial.print("displayMode: ");
  Serial.println(displayMode);

  digitalWrite(CheckPin1, HIGH);
  
  switch (displayMode) {
  case DM_TITLE:
    break;
  case DM_TITLE_STR1:
    for (int i = 0; i < MASTER_TITLE_STR_LEN; i++) {
      masterTitleStr1[i] = Wire.read();
    }
    break;
  case DM_TITLE_STR2:
    for (int i = 0; i < MASTER_TITLE_STR_LEN; i++) {
      masterTitleStr2[i] = Wire.read();
    }
    break;
  case DM_TITLE_STR3:
    for (int i = 0; i < MASTER_TITLE_STR_LEN; i++) {
      masterTitleStr3[i] = Wire.read();
    }
    break;
  case DM_NORMAL:
    for (int i = 0; i < OSC_NUM; i++) {
      waveShape[i] = Wire.read();
    }
    for (int i = 0; i < OSC_NUM; i++) {
      frequencyRange[i] = Wire.read();
    }
    //fps = ((uint16_t)Wire.read() << 1) | Wire.read();
    fpsl = Wire.read();
    fpsh = Wire.read();
    fps = ((uint16_t)fpsh << 8) | fpsl;
    batteryVoltage = Wire.read();
    adcAvailable = Wire.read();
    break;
  /*
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
  */
  }
  
  digitalWrite(CheckPin1, LOW);
  
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

