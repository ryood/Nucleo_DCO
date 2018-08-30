/*
 * Nucleo DCO Test02
 *
 * 2018.08.28
 *
 */

#include "mbed.h"
#include "u8g2.h"

#define TITLE_STR1  ("Nucleo DCO Test02")
#define TITLE_STR2  ("2018.08.28")

uint8_t u8x8_gpio_and_delay_mbed(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
u8g2_t myScreen;

//-------------------------------------------------------------------------------------------------------------
// Main Routine
//
void u8g2Initialize()
{
	//u8g2_Setup_ssd1306_i2c_128x64_noname_f(&myScreen, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_mbed);
	u8g2_Setup_ssd1306_i2c_128x32_univision_f(&myScreen, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_mbed);
	u8g2_InitDisplay(&myScreen);
	u8g2_SetPowerSave(&myScreen, 0);
	u8g2_ClearBuffer(&myScreen);

	u8g2_SetFont(&myScreen, u8g2_font_10x20_mf);
	u8g2_DrawStr(&myScreen, 0, 16, TITLE_STR1);
	u8g2_DrawStr(&myScreen, 0, 32, TITLE_STR2);
	u8g2_SendBuffer(&myScreen);
	wait(3.0);
}

int main()
{
	u8g2Initialize();

	while(1) {
		u8g2_ClearBuffer(&myScreen);
		u8g2_SetFont(&myScreen, u8g2_font_10x20_mf);
		u8g2_DrawStr(&myScreen, 4, 16, "SQR SWD TRI");
		u8g2_DrawStr(&myScreen, 4, 32, "32' 8'  2' ");
		u8g2_SendBuffer(&myScreen);
		wait(1.0);
	}
}