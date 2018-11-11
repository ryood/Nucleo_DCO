/*
 * Nucleo DCO IC間通信 データーフォーマット
 *
 * 2018.11.11
 *
 */

#ifndef _DATA_COM_FORMAT_H_
#define _DATA_COM_FORMAT_H_

#define OSC_NUM  (3)

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

// I2C通信用構造体
// DM_NORMAL
#pragma pack(1)
struct normalData {
	uint8_t  waveShape[OSC_NUM];
	uint8_t  frequencyRange[OSC_NUM];
	uint16_t fps;
	uint8_t  batteryVoltage;
	bool     adcAvailable;
};

// DM_FREQUENCY
#pragma pack(1)
struct frequencyData {
  uint16_t rate[OSC_NUM];
  int16_t detune[OSC_NUM];
};

// DM_AMPLITUDE
#pragma pack(1)
struct amplitudeData {
  int16_t amplitude[OSC_NUM];
  int16_t masterAmplitude;
  uint8_t clip;
};

// DM_PULSE_WIDTH
#pragma pack(1)
struct pulseWidthData {
  int16_t pulseWidth[OSC_NUM];
};

#endif //_DATA_COM_FORMAT_H_
