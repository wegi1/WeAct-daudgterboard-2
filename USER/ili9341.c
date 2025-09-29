/*
 * ili9341.c
 *

from Original source:

MIT License


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

#include <ili9341.h>
#include "main.h" // Hardware setting

// This function is for compatible HiLetgo ILI9341
enum {
  MemoryAccessControlNormalOrder,
  MemoryAccessControlReverseOrder
} MemoryAccessControlRefreshOrder;

enum {
	MemoryAccessControlColorOrderRGB,
	MemoryAccessControlColorOrderBGR
} MemoryAccessControlColorOrder;
//=================================================================

//=================================================================
extern void Error_Handler(void);
extern SPI_HandleTypeDef hspi1;


//===

uint8_t LCD_WORK_ORIENTATION = LCD_ORIENTATION_LANDSCAPE;
uint8_t LCD_NOT_WORK_ORIENTATION ;
uint8_t LCD_PORTRAIT_WORK_ORIENTATION ;
uint8_t LCD_PORTRAIT_NOT_WORK_ORIENTATION ;
//==================================
static void CS_L(void);
static void DC_L(void);
static void DC_H(void);

//==================================

extern TIM_HandleTypeDef htim2;
//===

//============================================================================================================================================

//============================================================================================================================================
//------------------------------------
#define   ILI_DISPOFF         0x28       // Display OFF
#define   ILI_PCA             0xCB       // Power Control A (5 parametr體 8b)
#define   ILI_PCB             0xCF       // Power Control B (3 parametry 8b)
#define   ILI_DTCA_ic         0xE8       // Driver Timming Control A (3 parametry 8b) - for internal clock
#define   ILI_POSC            0xED       // Power On Sequence Control (4 parametry 8b)
#define   ILI_DTCB            0xEA       // Driver Timming Control B (2 parametry 8b)
#define   ILI_E3G             0xF2       // Enable 3G (1 parametr 8b)
#define   ILI_PRC             0xF7       // Pump Ratio Control (1 parametr 8b)
#define   ILI_PWCTRL1         0xC0       // Power Control 1 (1 parametr 8b)
#define   ILI_PWCTRL2         0xC1       // Power Control 2 (1 parametr 8b)
#define   ILI_VMCTRL1         0xC5       // VCOM Control 1 (2 parametry 8b)
#define   ILI_VMCTRL2         0xC7       // VCOM Control 2 (1 parametr 8b)
#define   ILI_MADCTL          0x36       // Memory Access Control (1 parametr 8b)
#define   ILI_PIXSET          0x3A       // COLMOD: Pixel Format Set (1 parametr 8b)
#define   ILI_FRMCTR1         0xB1       // Frame Rate Control (In Normal Mode/Full Colors) (2 parametry 8b)
#define   ILI_GAMSET          0x26       // Gamma Set (1 parametr 8b)
#define   ILI_PGAMCTRL        0xE0       // Positive Gamma Correction (15 parametr體 8b)
#define   ILI_NGAMCTRL        0xE1       // Negative Gamma Correction (15 parametr體 8b)
#define   ILI_DISPON          0x29       // Display ON
#define   ILI_CASET           0x2A       // Column Address Set (4 parametry 8b: SC[15..8], SC[7..0], EC[15..8], EC[7..0])
#define   ILI_PASET           0x2B       // Page (row) Address Set (4 parametry 8b: SC[15..8], SC[7..0], EC[15..8], EC[7..0])
#define   ILI_RAMWR           0x2C       // Memory Write (n parametr體 18b, wywo硑wana bez parametr體 po CASET i PASET)
#define   ILI_ETMOD           0xB7       // Entry Mode Set (1 parametr 8b)
#define   ILI_DISCTRL         0xB6       // Display Function Control (4 parametry 8b)
#define   ILI_SLPOUT          0x11       // Sleep Out
#define   LCD_DISPLAY_ON      0x29       /* Display on register */
#define   LCD_GRAM            0x2C       /* GRAM register */

/*---------------------------------------------------------------------*/
//------------------------------------------------------------------------------------------------------------------------------------
static const uint8_t tab_orientations[] = {0x68, 0xe8, 0xa8, 0x28, 0x08, 0x48, 0x88, 0xc8};
//------------------------------------------------------------------------------------------------------------------------------------
/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static const uint8_t init_tab[] = {
        ILI_PCB, 3, 0x00, 0xC1, 0X30, /* 0xCF */ \
        ILI_POSC, 4, 0x64, 0x03, 0X12, 0X81, /* 0xED */  \
        ILI_DTCA_ic, 3, 0x85, 0x10, 0x7A, /* 0xE8 */ \
        ILI_PCA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02, /* 0xCB */ \
        ILI_PRC,1, 0x20, /* 0xF7 */ \
        ILI_DTCB, 2, 0x00, 0x00, /* 0xEA */ \
        ILI_PWCTRL1, 1, 0x1B, /* 0xC0 */ \
        ILI_PWCTRL2,1, 0x01, /* 0xC1 */ \
        ILI_VMCTRL1, 2, 0x30, 0x30, /* 0xC5 */ \
        ILI_VMCTRL2, 1, 0XB7, /* 0xC7 */ \
        ILI_MADCTL, 1, 0x48, /* 0x36 */ \
        ILI_PIXSET, 1, 0x55, /* 0x3A */ \
//		ILI9341_INTERFACECONTROL, 3, 0x01, 0x00, 0x20, // Little Endian for TouchGFX
        ILI_DISCTRL, 2, 0x0A, 0xA2, /* 0xB6 */  \
        ILI_E3G, 1, 0x00, /* 0xF2 */ \
        ILI_GAMSET, 1, 0x01, /* 0x26 */ \
        ILI_PGAMCTRL, 15, 0x0F, 0x2A, 0x28, 0x08, 0x0E, 0x08, 0x54, 0XA9, 0x43, 0x0A, 0x0F, 0x00, 0x00, 0x00, 0x00,  \
        ILI_NGAMCTRL, 15, 0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56, 0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F,  \
        ILI_PASET, 4, 0x00, 0x00, 0x01, 0x3f, /* 0x2B */ \
        ILI_CASET, 4, 0x00, 0x00, 0x00, 0xef, /* 0x2A */ \
        ILI_SLPOUT, 120, /* 0x11 */ \
        ILI_DISPON, 0, /* 0x29 */ \
		ILI_FRMCTR1, 2, 0x01, 0x12, /* 0xB1 */ \
        0x00
};

//====================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------

lcdPropertiesTypeDef  lcdProperties = { ILI9341_PIXEL_WIDTH, ILI9341_PIXEL_HEIGHT, LCD_ORIENTATION_PORTRAIT,true, true, 0x08 };

//====================================================================================================================================

//************************************************************************************************************************************



//========================================================================

/*****************************************************************************************************
 *
 * initialize sequence ILI_9341
 *
 ***************************************************************************************************/
void LCD_ILI9341_init(void)
{
    uint32_t i = 0;
    uint32_t i2;
    uint8_t counter, cmd;

//---
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // enable PWM back light
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);

    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(15);
	CS_L();
	HAL_Delay(50);
//---
	cmd = 0x01; //Software reset
	DC_L();
	if (HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
	HAL_Delay(50);
//---

    while(init_tab[i] != 0)
    {
    	LCD_WR_REG(init_tab[i]) ;
        counter = init_tab[i+1];
        i += 2;
        if(counter > 15)
        {
            HAL_Delay(counter);
        }
        else if(counter > 0)
        {
            for(i2 = 0; i2 < counter ; i2++)
            {
            	LCD_WR_DATA ( init_tab[i+i2]) ;
            }
            i += counter;
        }
    }

    //--------------
    lcdSetOrientation(lcdProperties.orientation);
    LCD_CLS(0x0000); // 0x1f init screen as blue
    //--------------
    HAL_Delay(22);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 26000);

    //--------------
}
//===================================================================================================
void lcdSetOrientation(lcdOrientationTypeDef value)
{

	uint16_t x,y;
	if(value > 7) { value = 0;}

	lcdProperties.orientation = value;
	lcdProperties.MADCTL_VAL = tab_orientations[value];

	y = ILI9341_PIXEL_WIDTH ;
	x = ILI9341_PIXEL_HEIGHT ;

	if(value < 4) {
		x = y;
		y = ILI9341_PIXEL_HEIGHT ;
	}

	lcdProperties.width = x;
	lcdProperties.height = y;
	LCD_WR_REG(ILI9341_MEMCONTROL);
	LCD_WR_DATA(lcdProperties.MADCTL_VAL);


	LCD_WORK_ORIENTATION = lcdProperties.orientation;
	if((LCD_WORK_ORIENTATION &3) != 0) {
		LCD_WORK_ORIENTATION = LCD_ORIENTATION_LANDSCAPE_ROTATE;
		LCD_NOT_WORK_ORIENTATION = LCD_ORIENTATION_LANDSCAPE ;
		LCD_PORTRAIT_WORK_ORIENTATION = LCD_ORIENTATION_PORTRAIT_ROTATE_MIRROR;
		LCD_PORTRAIT_NOT_WORK_ORIENTATION = LCD_ORIENTATION_PORTRAIT;
	} else {
		LCD_WORK_ORIENTATION = LCD_ORIENTATION_LANDSCAPE;
		LCD_NOT_WORK_ORIENTATION = LCD_ORIENTATION_LANDSCAPE_ROTATE ;
		LCD_PORTRAIT_WORK_ORIENTATION = LCD_ORIENTATION_PORTRAIT;
		LCD_PORTRAIT_NOT_WORK_ORIENTATION = LCD_ORIENTATION_PORTRAIT_ROTATE_MIRROR;
	}

}
//===================================================================================================
void LCD_CLS(uint16_t color){

	uint16_t data[320];
	uint32_t i;
	uint16_t color2;

	color2 = color << 8;
	color2 = color2 | (color >> 8);
	for(i = 0 ; i < 320 ; i++) { data[i] = color2;}

	//lcdSetOrientation(lcdProperties.orientation);
	LCD_OpenWin(0,0,lcdProperties.width,lcdProperties.height);

	for( i = 0 ; i < 240 ; i++) {
		HAL_SPI_Transmit(&hspi1, (uint8_t*) &data[0], 640, 1000);
	}
}
//=================================================

void LCD_OpenWin(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y)
{
	// Set Window
	LCD_WR_REG(ILI_CASET);
	LCD_WR_DATA(start_x >> 8);
	LCD_WR_DATA(0xFF & start_x);
	LCD_WR_DATA(end_x >> 8);
	LCD_WR_DATA(0xFF & end_x);

	LCD_WR_REG(ILI_PASET);
	LCD_WR_DATA(start_y >> 8);
	LCD_WR_DATA(0xFF & start_y);
	LCD_WR_DATA(end_y >> 8);
	LCD_WR_DATA(0xFF & end_y);
	LCD_WR_REG(ILI_RAMWR);
}
//=================================================
void LCD_Put_Pixel(u16 x, u16 y, u16 Color)
{

	LCD_OpenWin(x,y,x,y);
	uint8_t data[2];
	data[0] = Color >> 8;
	data[1] = Color ;

	if (HAL_SPI_Transmit(&hspi1, (u8*) &data[0], 2, 1000) != HAL_OK) {
		Error_Handler();
	}
}
//=================================================
void LCD_WR_REG(uint8_t data)
{
	DC_L();
	if (HAL_SPI_Transmit(&hspi1, &data, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
	DC_H();
}
//=================================================
void LCD_WR_DATA(uint8_t data)
{
	if (HAL_SPI_Transmit(&hspi1, &data, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
}
//=================================================
static void CS_L(void)
{
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
}
//=================================================
static void DC_L(void)
{
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
}
//=================================================
static void DC_H(void)
{
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
}
//=================================================

//=========================================================================

//=========================================================================

