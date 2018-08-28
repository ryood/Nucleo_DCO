#include "mbed.h"
#include "u8g2.h"

DigitalOut led(D13);

uint8_t u8x8_gpio_and_delay_mbed(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
u8g2_t myScreen;

extern const uint8_t XLargeFont[] U8G2_FONT_SECTION("XLargeFont");
extern const uint8_t LargeFont[] U8G2_FONT_SECTION("LargeFont");

void setup() {
  //u8g2_Setup_ssd1306_i2c_128x64_noname_f(&myScreen, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_mbed);
  u8g2_Setup_ssd1306_i2c_128x32_univision_f(&myScreen, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_mbed);
  u8g2_InitDisplay(&myScreen);
  u8g2_SetPowerSave(&myScreen, 0);
  u8g2_ClearBuffer(&myScreen);

  // u8g2_SetFont(&myScreen, XLargeFont);
  // u8g2_DrawUTF8(&myScreen, 0, 32, "53");
  // u8g2_SetFont(&myScreen, LargeFont);
  // u8g2_DrawUTF8(&myScreen, 0, 32, "Hello world");
  // u8g2_SetFont(&myScreen, u8g2_font_f16_t_japanese1);
  // u8g2_DrawUTF8(&myScreen, 0, 20, "こんにちは世界");
  // u8g2_SetFont(&myScreen, u8g2_font_unifont_t_chinese2);
  // u8g2_DrawUTF8(&myScreen, 0, 20, "你好世界");

  u8g2_SetFont(&myScreen, u8g2_font_10x20_mf);
  u8g2_DrawStr(&myScreen, 4, 16, "SQR SWD TRI");
  u8g2_DrawStr(&myScreen, 4, 32, "32' 8'  2' ");

  u8g2_SendBuffer(&myScreen);
}

int main() {
  led = 0;
  // printf("Hello world");
  setup();
  
  while(1) {
    led = !led;
    wait(1.0);
  }
  return 0;
}