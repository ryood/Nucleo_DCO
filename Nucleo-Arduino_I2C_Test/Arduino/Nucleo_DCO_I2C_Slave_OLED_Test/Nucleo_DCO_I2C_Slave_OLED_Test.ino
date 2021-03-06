/*
 * Nucleo DCO I2C Slave / OLED Module Test.
 *
 * 2018.11.07
 *
 */

#include <Wire.h>
#include <U8g2lib.h>
#include "DataComFormat.h"

#define UART_TRACE  (1)

#define TITLE_STR1  ("I2C Slave Test")
#define TITLE_STR2  ("20181111")

#define I2C_ADDR   (0x08)
#define I2C_CLOCK  (100000)

#define MASTER_TITLE_STR_LEN (32)

//U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

const int CheckPin1 = 2;

// Const strings
const char* waveShapeName[] = {
  "SIN",
  "TRI",
  "SWU",
  "SWD",
  "SQR",
  "NOS",
  "XXX"
};

const char* frequencyRangeName[] = {
  "A-1",
  " A0",
  " A1",
  " A2",
  " A3",
  " A4",
  " A5",
  " A6",
  " A7",
  " A8"
};

char masterTitleStr1[MASTER_TITLE_STR_LEN] = "TitleStr1";
char masterTitleStr2[MASTER_TITLE_STR_LEN] = "TitleStr2";
char masterTitleStr3[MASTER_TITLE_STR_LEN] = "TitleStr3";

volatile int displayMode = DM_TITLE;

volatile struct normalData normalData;
volatile struct frequencyData frequencyData;
volatile struct amplitudeData amplitudeData;
volatile struct pulseWidthData pulseWidthData;
volatile bool displayOffMessage;

void setup()
{
#if (UART_TRACE)  
  Serial.begin(9600);           // start serial for output
  Serial.println();
  Serial.println(TITLE_STR1);
  Serial.println(TITLE_STR2);
#endif

  pinMode(CheckPin1, OUTPUT);
  
  // OLED
  u8g2.begin();
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_8x13_tf);
  do {
    u8g2.drawStr(0,10,TITLE_STR1);
    u8g2.drawStr(0,25,TITLE_STR2);
  } while (u8g2.nextPage());
  
  // I2C
  Wire.begin(I2C_ADDR);         // join i2c bus with address #8
  pinMode(A4, INPUT);           // disable pullup
  pinMode(A5, INPUT);           // disable pullup
  Wire.setClock(I2C_CLOCK);
  Wire.onReceive(receiveEvent); // register event

  delay(2000);
}

//-------------------------------------------------------------------------------------------------
// OLED Display
//

// n倍数の整数部を求める
// param
//     n: 変換する整数
// 　　d: 小数点以下桁数
int getIntg(int n, int d)
{
  int ret = n / d;
  return ret;
}

// n倍数の少数部を求める
// param
//     n: 変換する整数
// 　　d: 小数点以下桁数
int getDeci(int n, int d)
{
  int ret = n % d;
  if (ret < 0) {
    ret = - ret;
  }
  return ret;
}

void displayTitle()
{
#if (UART_TRACE)
  Serial.println("displayTitle()");
#endif

  u8g2.firstPage();
  u8g2.setFont(u8g2_font_10x20_mf);
  do {
    u8g2.drawStr(0, 16, masterTitleStr1);
    u8g2.drawStr(0, 32, masterTitleStr2);
    u8g2.drawStr(0, 48, masterTitleStr3);
  } while (u8g2.nextPage());
}

void displayNormal()
{
 #if (UART_TRACE)
  Serial.println("displayNormal()");
#endif

  char strBuffer[20];
  
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_10x20_mf);
  do {
    sprintf(strBuffer, "%s %s %s", 
      waveShapeName[normalData.waveShape[0]],
      waveShapeName[normalData.waveShape[1]],
      waveShapeName[normalData.waveShape[2]]
    );
    u8g2.drawStr(4, 16, strBuffer);

    sprintf(strBuffer, "%s %s %s", 
      frequencyRangeName[normalData.frequencyRange[0]],
      frequencyRangeName[normalData.frequencyRange[1]],
      frequencyRangeName[normalData.frequencyRange[2]]
    );
    u8g2.drawStr(4, 32, strBuffer);

    int fps_intg = getIntg(normalData.fps, 10);
    int fps_deci = getDeci(normalData.fps, 10);
    sprintf(strBuffer, "FPS:%d.%d", fps_intg, fps_deci);
    u8g2.drawStr(4, 48, strBuffer);

    int bat_intg = getIntg(normalData.batteryVoltage, 10);
    int bat_deci = getDeci(normalData.batteryVoltage, 10);
    sprintf(strBuffer, "BAT:%d.%dV %s", bat_intg, bat_deci, normalData.adcAvailable ? "xx" : "AD");
    u8g2.drawStr(4, 64, strBuffer);
  } while (u8g2.nextPage()); 
}

void displayFrequency()
{
#if (UART_TRACE)
  Serial.println("displayFrequency()");
#endif

  char strBuffer[20];
  
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_10x20_mf);
  do {
    sprintf(strBuffer, "F1:%d.%d Hz", getIntg(frequencyData.rate[0], 10), getDeci(frequencyData.rate[0], 10)); 
    u8g2.drawStr(0, 16, strBuffer);
    sprintf(strBuffer, "F2:%d.%d Hz", getIntg(frequencyData.rate[1], 10), getDeci(frequencyData.rate[1], 10)); 
    u8g2.drawStr(0, 32, strBuffer);
    sprintf(strBuffer, "F3:%d.%d Hz", getIntg(frequencyData.rate[2], 10), getDeci(frequencyData.rate[2], 10)); 
    u8g2.drawStr(0, 48, strBuffer);
    
    // Detune
    sprintf(strBuffer, "%d.%d %d.%d", 
      getIntg(frequencyData.detune[1], 1000), getDeci(frequencyData.detune[1], 1000),
      getIntg(frequencyData.detune[2], 1000), getDeci(frequencyData.detune[2], 1000)
    );
    u8g2.drawStr(0, 64, strBuffer);
  } while (u8g2.nextPage()); 
}

void displayAmplitude()
{
#if (UART_TRACE)
  Serial.println("displayAmplitude()");
#endif

  char strBuffer[20];
  
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_10x20_mf);
  do {
    sprintf(strBuffer, "AMP1: %d.%d ", getIntg(amplitudeData.amplitude[0], 1000), getDeci(amplitudeData.amplitude[0], 1000));
    u8g2.drawStr(0, 16, strBuffer);
    sprintf(strBuffer, "AMP2: %d.%d ", getIntg(amplitudeData.amplitude[1], 1000), getDeci(amplitudeData.amplitude[1], 1000));
    u8g2.drawStr(0, 32, strBuffer);
    sprintf(strBuffer, "AMP3: %d.%d ", getIntg(amplitudeData.amplitude[2], 1000), getDeci(amplitudeData.amplitude[2], 1000));
    u8g2.drawStr(0, 48, strBuffer);
    sprintf(strBuffer, "MAMP: %d.%d ", getIntg(amplitudeData.masterAmplitude, 1000), getDeci(amplitudeData.masterAmplitude, 1000));
    u8g2.drawStr(0, 64, strBuffer);    
  } while (u8g2.nextPage());   
}

void displayPulseWidth()
{
#if (UART_TRACE)
  Serial.println("displayPulseWidth()");
#endif

  char strBuffer[20];
  
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_10x20_mf);
  do {
    sprintf(strBuffer, "PW1: %d.%d ", getIntg(pulseWidthData.pulseWidth[0], 1000), getDeci(pulseWidthData.pulseWidth[0], 1000));
    u8g2.drawStr(0, 16, strBuffer);
    sprintf(strBuffer, "PW2: %d.%d ", getIntg(pulseWidthData.pulseWidth[1], 1000), getDeci(pulseWidthData.pulseWidth[1], 1000));
    u8g2.drawStr(0, 32, strBuffer);
    sprintf(strBuffer, "PW3: %d.%d ", getIntg(pulseWidthData.pulseWidth[2], 1000), getDeci(pulseWidthData.pulseWidth[2], 1000));
    u8g2.drawStr(0, 48, strBuffer);
  } while (u8g2.nextPage());   
}

void displayOff()
{
#if (UART_TRACE)
  Serial.println("displayOff()");
#endif

  char strBuffer[20];
  
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_10x20_mf);
  do {
    sprintf(strBuffer, "DISPLAY OFF"); 
    u8g2.drawStr(4, 24, strBuffer);  
  } while (u8g2.nextPage()); 
}

//-------------------------------------------------------------------------------------------------
// Main loop
//
void loop()
{
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
  
#if (UART_TRACE)  
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
    Serial.print("WaveShpe1: ");   Serial.println(normalData.waveShape[0]);
    Serial.print("WaveShpe2: ");   Serial.println(normalData.waveShape[1]);
    Serial.print("WaveShpe3: ");   Serial.println(normalData.waveShape[2]);
    Serial.print("FreqRange1: ");  Serial.println(normalData.frequencyRange[0]);
    Serial.print("FreqRange2: ");  Serial.println(normalData.frequencyRange[1]);
    Serial.print("FreqRange3: ");  Serial.println(normalData.frequencyRange[2]);
    Serial.print("Fps: ");         Serial.println(normalData.fps);
    Serial.print("Fps: ");         Serial.print(normalData.fps/10); Serial.print("."); Serial.println(normalData.fps%10); 
    Serial.print("BattVoltage: "); Serial.println(normalData.batteryVoltage);
    Serial.print("BattVoltage: "); Serial.print(normalData.batteryVoltage/10); Serial.print("."); Serial.println(normalData.batteryVoltage%10);
    Serial.print("ADCAvilable: "); Serial.println(normalData.adcAvailable);
    Serial.println("*******************************");
    break;
  case DM_FREQUENCY:
    Serial.println("********** Frequency **********");
    Serial.print("Rate1: ");   Serial.println(frequencyData.rate[0]);
    Serial.print("Rate1: ");   Serial.print(frequencyData.rate[0]/10); Serial.print("."); Serial.println(frequencyData.rate[0]%10);
    Serial.print("Rate2: ");   Serial.println(frequencyData.rate[1]);
    Serial.print("Rate2: ");   Serial.print(frequencyData.rate[1]/10); Serial.print("."); Serial.println(frequencyData.rate[1]%10);
    Serial.print("Rate3: ");   Serial.println(frequencyData.rate[2]);
    Serial.print("Rate3: ");   Serial.print(frequencyData.rate[2]/10); Serial.print("."); Serial.println(frequencyData.rate[2]%10);
    Serial.print("Detune1: "); Serial.println(frequencyData.detune[0]);
    Serial.print("Detune1: "); Serial.print(frequencyData.detune[0]/1000); Serial.print("."); Serial.println(frequencyData.detune[0]%1000);
    Serial.print("Detune2: "); Serial.println(frequencyData.detune[1]);
    //Serial.print("Detune2: "); Serial.print(frequencyData.detune[1]/1000); Serial.print("."); Serial.println(frequencyData.detune[1]%1000);
    Serial.print("Detune2: "); Serial.println((float)(frequencyData.detune[1])/1000.0f, 3);
    Serial.print("Detune3: "); Serial.println(frequencyData.detune[2]);
    Serial.print("Detune3: "); Serial.print(frequencyData.detune[2]/1000); Serial.print("."); Serial.println(frequencyData.detune[2]%1000);
    Serial.println("*******************************");
    break;
  case DM_AMPLITUDE:
    Serial.println("********** Amplitude **********");
    Serial.print("Amplitude1: ");      Serial.println(amplitudeData.amplitude[0]);
    Serial.print("Amplitude1: ");      Serial.println((float)(amplitudeData.amplitude[0])/1000.0f, 3);
    Serial.print("Amplitude2: ");      Serial.println(amplitudeData.amplitude[1]);
    Serial.print("Amplitude2: ");      Serial.println((float)(amplitudeData.amplitude[1])/1000.0f, 3);
    Serial.print("Amplitude3: ");      Serial.println(amplitudeData.amplitude[2]);
    Serial.print("Amplitude3: ");      Serial.println((float)(amplitudeData.amplitude[2])/1000.0f, 3);
    Serial.print("MasterAmplitude: "); Serial.println(amplitudeData.masterAmplitude);
    Serial.print("MasterAmplitude: "); Serial.println((float)(amplitudeData.masterAmplitude)/1000.0f, 3);
    Serial.print("Clip: ");            Serial.println(amplitudeData.clip); 
    Serial.println("*******************************");
    break;
  case DM_PULSE_WIDTH:
    Serial.println("********* Pulse Width *********");
    Serial.print("PulseWidth1: ");     Serial.println(pulseWidthData.pulseWidth[0]);
    Serial.print("PulseWidth1: ");     Serial.println((float)(pulseWidthData.pulseWidth[0])/1000.0f, 3);
    Serial.print("PulseWidth2: ");     Serial.println(pulseWidthData.pulseWidth[1]);
    Serial.print("PulseWidth2: ");     Serial.println((float)(pulseWidthData.pulseWidth[1])/1000.0f, 3);
    Serial.print("PulseWidth3: ");     Serial.println(pulseWidthData.pulseWidth[2]);
    Serial.print("PulseWidth3: ");     Serial.println((float)(pulseWidthData.pulseWidth[2])/1000.0f, 3);
    Serial.println("*******************************");
    break;
  case DM_DISPLAY_OFF:
    Serial.println("********* Display Off *********");
    Serial.print("DisplayOffMessage: "); Serial.println(displayOffMessage);
    Serial.println("*******************************");
  }
  delay(1000);
#endif
}

//-------------------------------------------------------------------------------------------------
// I2C Event
//
void receiveEvent(int byteN)
{
#if (UART_TRACE)  
  Serial.println("receiveEvent()");
#endif
  
  displayMode = Wire.read();

#if (UART_TRACE)  
  Serial.print("displayMode: ");
  Serial.println(displayMode);
#endif

  digitalWrite(CheckPin1, HIGH);

  uint8_t* p;
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
    p = (uint8_t *)&normalData;
    for (int i = 0; i < sizeof(struct normalData); i++) {
      p[i] = Wire.read();
    }
  case DM_FREQUENCY:
    p = (uint8_t *)&frequencyData;
    for (int i = 0; i < sizeof(struct frequencyData); i++) {
      p[i] = Wire.read();
    }
    break;
  case DM_AMPLITUDE:
    p = (uint8_t *)&amplitudeData;
    for (int i = 0; i < sizeof(struct amplitudeData); i++) {
      p[i] = Wire.read();
    }
    break;
  case DM_PULSE_WIDTH:
    p = (uint8_t *)&pulseWidthData;
    for (int i = 0; i < sizeof(struct pulseWidthData); i++) {
      p[i] = Wire.read();
    }
    break;
  case DM_DISPLAY_OFF:
    displayOffMessage = Wire.read();
    break;
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

