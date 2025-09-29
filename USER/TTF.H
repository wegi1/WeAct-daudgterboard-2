/*
 * TTF.H
 *
 *  Created on: Sep 1, 2025
 *      Author: BOLO
 */

#ifndef TTF_H_
#define TTF_H_

//================================================================================

#include "stdio.h"
#include "stb_truetype.h"
#include "string.h"
#include "fonty.h"
#include "dum.h"
#include "ili9341.h"
#include "GUI.h"
#include "XPT2046_touch.h"
//================================================================================
const uint16_t RGB565_GRAYSCALE_32[] =
{ 0x0000, 0x0841, 0x1062, 0x18C3, 0x2104, 0x2945, 0x3166, 0x39C7, 0x4208, 0x4A49, 0x52AA, 0x5AAB, 0x630C, 0x6B4D, 0x738e, 0x7bcf, 0x8410, 0x8c51, 0x94b2, 0x9cf3, 0xa514, 0xAd55, 0xb596, 0xbdd7, 0xc638, 0xce79, 0xd6ba, 0xdedb, 0xe73c, 0xef5d, 0xf7be, 0xFFFF };

extern lcdPropertiesTypeDef  lcdProperties ;
extern MATRIX  matrix ;
//================================================================================
stbtt_fontinfo font;  // łatwiejszy dostęp :P

char tekst[30];

int efect_dx = 1;
uint8_t fade_effect = 0;

extern uint8_t LCD_WORK_ORIENTATION;
extern uint8_t LCD_NOT_WORK_ORIENTATION ;
extern uint8_t LCD_PORTRAIT_WORK_ORIENTATION ;
extern uint8_t LCD_PORTRAIT_NOT_WORK_ORIENTATION ;

extern uint8_t idx[] ;
extern uint8_t idy[] ;

uint16_t lcd_text_color = COLOR_565_WHITE;
uint16_t lcd_background_color = COLOR_565_BLACK;

extern void LCD_OpenWin(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);



extern SPI_HandleTypeDef hspi1;
//===========================================================================================
#define delay(x) HAL_Delay(x)

//=====================================================================================================================================

//===================================================================================
/**
 * @brief Fast draw bitmap rgb565
 * @param x
 * @param y
 * @param szerokosc
 * @param wysokosc
 * @param data
 */
extern void lcd_Draw16BitBitmap(uint16_t x, uint16_t y, uint16_t szerokosc, uint16_t wysokosc, const uint16_t *data)
{

	uint8_t  TXT_COL[2];
	uint8_t  BCG_COL[2];

	TXT_COL[0] = lcd_text_color >> 8;
	TXT_COL[1] = lcd_text_color ;

	BCG_COL[0] = lcd_background_color >> 8;
	BCG_COL[1] = lcd_background_color ;

	LCD_OpenWin(x, y, x + szerokosc - 1, y + wysokosc - 1);

	for (uint32_t t = 0; t < szerokosc * wysokosc; t++)
		if(data[t] != 0){

			if (HAL_SPI_Transmit(&hspi1, (u8*) &TXT_COL[0], 2, 1000) != HAL_OK) {
				Error_Handler();
			}
		} else {
			if (HAL_SPI_Transmit(&hspi1, (u8*) &BCG_COL[0], 2, 1000) != HAL_OK) {
				Error_Handler();
			}
		}
}
//===================================================================================

/**
 * @brief Kolor tekstu w formacie rgb565.
 * @param kolor
 */
extern void lcd_set_text_color(uint16_t kolor)
{
	lcd_text_color = kolor;
}
//===================================================================================

/**
 * @brief Kolor tła tekstu w formacie rgb565.
 * @param kolor
 */
extern void lcd_set_background_color(uint16_t kolor)
{
	lcd_background_color = kolor;
}
//===================================================================================
extern int render_biggestFont_portait(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{
	//lcd_Direction(ROTATE_0);
	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
	{
		return 0;  // nie udało sie zainicjować fontów :(
	}

	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
	int baseline = (int) (ascent * scale);

	// 🔹 Ustalamy stały rozmiar glifu (monospace)
	int adv_width, lsb;
	stbtt_GetCodepointHMetrics(&font, 'M', &adv_width, &lsb);  // szerokość największego znaku
	int mono_w = (int) (adv_width * scale);
	int text_len = strlen(text);

	int x0, y0, x1, y1;
	stbtt_GetCodepointBitmapBox(&font, 'M', scale, scale, &x0, &y0, &x1, &y1);
	int total_h = baseline + y1;



	// 🔹 Renderowanie znaków jeden po drugim
	int pen_x = 0;
	for (int i = 0; i < text_len; i++)
	{
		int cp = text[i];
		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBox(&font, cp, scale, scale, &x0, &y0, &x1, &y1);

		int gw = x1 - x0;
		int gh = y1 - y0;
		unsigned char *g_bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, cp, &gw, &gh, 0, 0);



uint16_t tx_color = lcd_text_color;
uint8_t tmpr1 = tx_color >> 8;
tx_color = tx_color << 8;
tx_color = tx_color + tmpr1;
uint16_t bk_color = lcd_background_color;
tmpr1 = bk_color >> 8;
bk_color = bk_color << 8;
bk_color += tmpr1;

my_utoa(&idx[0], gh);
my_utoa(&idy[0], gw);

uint16_t buforek[240];

//#define CCM_ADDRESS 0xD0000000
//uint32_t* pFBaddr = (uint32_t *) FRAME_ADDRESS;

        LCD_OpenWin(0, (baseline + y0), 239, (baseline + y0 + gh));
		for (int row = 0; row < gh; row++)
		{
			for(uint8_t x = 0; x < 240 ; x++) { buforek[x] = bk_color  ;}
			for (int col = 0; col < gw; col++)
			{
				unsigned char val = g_bitmap[row * gw + col];
				int px = pen_x + col + (mono_w - gw) ;  // centrowanie w polu monospace
//				int py = baseline + y0 + row;
				if (val > 0)
				{
					//LCD_Put_Pixel(px, py, lcd_text_color);
					buforek[px] = tx_color;
				}
			}
			HAL_SPI_Transmit(&hspi1, (u8*) &buforek[0], 480, 1000);
		}

		stbtt_FreeBitmap(g_bitmap, NULL);
		pen_x += mono_w;
	}

	return total_h;
}

//================================================================================
extern int render_text_monospace(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{

	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
	{
		return 0;  // nie udało sie zainicjować fontów :(
	}

	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
	int baseline = (int) (ascent * scale);

	// 🔹 Ustalamy stały rozmiar glifu (monospace)
	int adv_width, lsb;
	stbtt_GetCodepointHMetrics(&font, 'M', &adv_width, &lsb);  // szerokość największego znaku
	int mono_w = (int) (adv_width * scale);
	int text_len = strlen(text);

	int x0, y0, x1, y1;
	stbtt_GetCodepointBitmapBox(&font, 'M', scale, scale, &x0, &y0, &x1, &y1);
	int total_h = baseline + y1;


	uint16_t tx_color = lcd_text_color;
	uint8_t tmpr1 = tx_color >> 8;
	tx_color = tx_color << 8;
	tx_color = tx_color + tmpr1;
	uint16_t bk_color ;
	bk_color = lcd_background_color;
	tmpr1 = bk_color >> 8;
	bk_color = bk_color << 8;
	bk_color += tmpr1;


	uint16_t buforek[320];
	for(uint32_t filler = 0; filler < 320; filler++) { buforek[filler] = bk_color ;}


	// 🔹 Renderowanie znaków jeden po drugim
	int pen_x = 0;
	for (int i = 0; i < text_len; i++)
	{
		int cp = text[i];
		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBox(&font, cp, scale, scale, &x0, &y0, &x1, &y1);

		int gw = x1 - x0;
		int gh = y1 - y0;
		unsigned char *g_bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, cp, &gw, &gh, 0, 0);

		LCD_OpenWin(pozx+pen_x, pozy,(pozx+pen_x+mono_w-1), pozy+gh-1); // open window big as TTF fonts

		for (int row = 0; row < gh; row++)
		{
			for (int col = 0; col < gw; col++)
			{
				unsigned char val = g_bitmap[row * gw + col];
				int px = pen_x + col + (mono_w - gw)  ;  // centrowanie w polu monospace

				if (val > 0)
				{
					buforek[px] = tx_color;
				}
				else
				{
					buforek[px] = bk_color;
				}
			}
			HAL_SPI_Transmit(&hspi1, (u8*) &buforek[pen_x], (mono_w << 1), 1000);
		}
		stbtt_FreeBitmap(g_bitmap, NULL);
		pen_x += mono_w;
	}

	return total_h;
}
//===================================================================================

//======================================================================================================================
//======================================================================================================================
extern int lcd_mono_text_boxed(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{
	return pozy + render_text_monospace(pozx, pozy, text, font_data, pixel_height);
}
//======================================================================================================================
//======================================================================================================================

//===================================================================================
// Główna funkcja renderująca tekst
//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
extern void lcd_text_bounds(stbtt_fontinfo *font, const char *text, float pixel_height, int *out_width, int *out_height)
{
	float scale = stbtt_ScaleForPixelHeight(font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(font, &ascent, &descent, &line_gap);

	int baseline = (int) (ascent * scale);
	int min_x = 999999, min_y = 999999;
	int max_x = -999999, max_y = -999999;

	int x = 0;
	for (const char *p = text; *p; p++)
	{

		int glyph = stbtt_FindGlyphIndex(font, *p);

		int ax, lsb;
		stbtt_GetGlyphHMetrics(font, glyph, &ax, &lsb);

		int x0, y0, x1, y1;
		stbtt_GetGlyphBitmapBox(font, glyph, scale, scale, &x0, &y0, &x1, &y1);

		int gx0 = x + x0;
		int gy0 = baseline + y0;
		int gx1 = x + x1;
		int gy1 = baseline + y1;

		if (gx0 < min_x) min_x = gx0;
		if (gy0 < min_y) min_y = gy0;
		if (gx1 > max_x) max_x = gx1;
		if (gy1 > max_y) max_y = gy1;

		int kern = stbtt_GetGlyphKernAdvance(font, glyph, *(p + 1));
		x += (int) (ax * scale) + (int) (kern * scale);
	}

	*out_width = (max_x - min_x);
	*out_height = (max_y - min_y) + 3;
}
//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
// TODO HERE WORK
//===================================================================================

/**
 * @brief procedura rysująca
 * @param pozx
 * @param pozy
 * @param text
 * @param font_data
 * @param pixel_height
 * @param fast
 * @param boxed
 */
int lcd_render_text(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height, uint8_t fast, uint8_t boxed)
{

	int text_width, text_height;

	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
	{
		return 0;  // nie udało sie zainicjować fontów :(
	}

	lcd_text_bounds(&font, text, pixel_height, &text_width, &text_height);
	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);

	int baseline = (int) (ascent * scale);
	int x = 0;

	uint16_t tx_color = lcd_text_color;
	uint8_t tmpr1 = tx_color >> 8;
	tx_color = tx_color << 8;
	tx_color = tx_color + tmpr1;
	uint16_t bk_color ;
	bk_color = lcd_background_color;
	tmpr1 = bk_color >> 8;
	bk_color = bk_color << 8;
	bk_color += tmpr1;


	uint16_t buforek[320] = {bk_color};



	for (const char *p = text; *p; p++)
	{
		int glyph = stbtt_FindGlyphIndex(&font, *p);

		int ax, lsb;
		stbtt_GetGlyphHMetrics(&font, glyph, &ax, &lsb);

		int x0, y0, x1, y1;
		stbtt_GetGlyphBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);

		int width = x1 - x0;
		int height = y1 - y0;

		unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, *p, &width, &height, &x0, &y0);

		LCD_OpenWin((pozx + x), (pozy+baseline + y0), (pozx + x+width-1), (pozy+baseline + y0 + height-1));

		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				uint8_t value = bitmap[row * width + col];
				if (value > 0)
				{
					buforek[col] = lcd_text_color;
//					LCD_Put_Pixel(pozx+x + col ,pozy+ (row + baseline + y0),lcd_text_color);
				}
				else
				{
					buforek[col] = lcd_background_color;
//					LCD_Put_Pixel(pozx+x + col , pozy+(row + baseline + y0),lcd_background_color);
				}
			}
			HAL_SPI_Transmit(&hspi1, (u8*) &buforek[0], (width << 1), 1000);
		}

		stbtt_FreeBitmap(bitmap, NULL);

		int kern = stbtt_GetGlyphKernAdvance(&font, glyph, *(p + 1));
		x += (int) (ax * scale) + (int) (kern * scale);
	}

	return text_height;
}
//===================================================================================

extern int lcd_text_boxed(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{
	return pozy + lcd_render_text(pozx, pozy, text, font_data, pixel_height, 0, 0);
}
//===================================================================================

//===================================================================================

extern int lcd_fade_text(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{
	uint16_t backup = lcd_text_color;
	lcd_set_text_color(RGB565_GRAYSCALE_32[fade_effect]);
	uint32_t tmp = pozy + lcd_render_text(pozx, pozy, text, font_data, pixel_height, 0, 0);
	lcd_set_text_color(backup);
	fade_effect += efect_dx;
	if ((fade_effect == 30) || fade_effect == 0) efect_dx = -efect_dx;
	return tmp;
}

//===================================================================================
extern void draw_End(void){
	if(lcdProperties.orientation > 3 ) { lcdSetOrientation( LCD_ORIENTATION_LANDSCAPE_ROTATE ); }
	lcd_text_boxed(170, 203, "CLS", digital_7_ttf, 25);
	lcd_text_boxed(250, 203, "END", digital_7_ttf, 25);
}
//===================================================================================

//===================================================================================
extern void Big_TTF_Demo(void)
{
uint8_t t;
	lcdSetOrientation(LCD_ORIENTATION_PORTRAIT);
	LCD_CLS(lcd_background_color);


	for (t=0;t<10;t++)
	{
		sprintf(tekst, "%d", t);

		render_biggestFont_portait(0, 0, tekst, digital_7_ttf, 230);

//		render_text_monospace(0, 0, tekst, digital_7_ttf, 230);
//		lcd_mono_text_boxed(2, 0, tekst, digital_7_ttf, 230);
//        while(HAL_GPIO_ReadPin(T_PEN_GPIO_Port, T_PEN_Pin) != 0){;}
		delay(100);
	}


}

//===================================================================================
extern void Timer_Demo(void) {

	lcdSetOrientation(LCD_ORIENTATION_LANDSCAPE_ROTATE);
	lcd_text_color = 0xffff;
	lcd_background_color = 0;
	LCD_CLS(lcd_background_color);

	sprintf(tekst, "%02d", 9);
	lcd_mono_text_boxed(2, 0, tekst, digital_7_ttf, 86);

	sprintf(tekst, "%02d", 43);
	lcd_mono_text_boxed(167, 0, tekst, digital_7_ttf, 86);

	LCD_DisASquare(158, 36, 12, lcd_text_color);
	LCD_DisASquare(158, 78, 12, lcd_text_color);


	for(int i = 0 ; i < 100 ; i++) {
		sprintf(tekst, "%02d",  i);
		lcd_mono_text_boxed(130, 150, tekst, digital_7_ttf, 40);
		HAL_Delay(5);
	}

//	HAL_Delay(200);

}
//===================================================================================
extern void live_show_ts(void)
{

	u16 x01, y01;
	POINT   COORDS ;
	POINT RAW_DATA ;
	uint8_t buff01[200] = {0};




	if(lcdProperties.orientation != 2) {
		lcdSetOrientation(0);
	}
	LCD_CLS(COLOR_565_BLACK);

	lcd_mono_text_boxed(10, 60, "PRESS TS", digital_7_ttf, 40);



	while(XPT2046_TouchPressed() == false){;} // wait for press touch screen

	LCD_CLS(COLOR_565_BLACK);

	lcd_text_boxed(250, 203, "END", digital_7_ttf, 25);
	LCD_DisARectangular(239,200,319,202, COLOR_565_WHITE);
	LCD_DisARectangular(239,200,241,239, COLOR_565_WHITE);

	draw_cross(160, 120, 0xffff);

	draw_cross(290, 30, 0xffff);
	draw_cross(30, 210, 0xffff);
	draw_cross(160, 210, 0xffff);
	draw_cross(30, 120, 0xffff);
	draw_cross(290, 120, 0xffff);

	while(1) {

		while(XPT2046_TouchPressed() == false){;} // wait for press touch screen
		while( XPT2046_GetFastCoordinates(&x01 , &y01) != true);

		RAW_DATA.x= x01   ;
		RAW_DATA.y= y01   ;

		HAL_Delay(2);

		my_utoa(&idx[0], RAW_DATA.x);
		my_utoa(&idy[0], RAW_DATA.y);


		LCD_OpenWin(20, 20, 115, 80);
		for(uint32_t ix = 0; ix < 57 ; ix++) {
			HAL_SPI_Transmit(&hspi1, &buff01[0], 200, 1000) ;
		}

        lcd_mono_text_boxed(20, 20, (char*)&idx[0], digital_7_ttf, 20);
		lcd_mono_text_boxed(20, 50, (char*)&idy[0], digital_7_ttf, 20);


		getDisplayPoint( &COORDS, &RAW_DATA, &matrix ) ;
//		take_displayPoint();

//extern POINT Paint_Display;




		if(COORDS.x > lcdProperties.width) { COORDS.x = lcdProperties.width-1;}
		if(COORDS.y > lcdProperties.height) { COORDS.x = lcdProperties.height-1;}

		if(lcdProperties.orientation == LCD_ORIENTATION_LANDSCAPE) {
			COORDS.x = 320 - COORDS.x;
			COORDS.y = 240 - COORDS.y;
		}
		my_utoa(&idx[0], COORDS.x);
		my_utoa(&idy[0], COORDS.y);

		LCD_OpenWin(120, 20, 215, 80);
		for(uint32_t ix = 0; ix < 57 ; ix++) {
			HAL_SPI_Transmit(&hspi1, &buff01[0], 200, 1000) ;
		}
        lcd_mono_text_boxed(120, 20, (char*)&idx[0], digital_7_ttf, 20);
		lcd_mono_text_boxed(120, 50, (char*)&idy[0], digital_7_ttf, 20);

		if(COORDS.y > 200) {
			if(COORDS.x > 240) {
				while(XPT2046_TouchPressed() == true ){;} // wait for releasse TS
				HAL_Delay(250);
				LCD_CLS(0);

				return;
			}
		}

	}
}
//===================================================================================
//=====================================================================================
int lcd_mono_t_box(int pozx, int pozy, const char *text, int data_font, float pixel_height)
{
	if(data_font == 0) {	return lcd_mono_text_boxed(pozx, pozy, text, digital_7_ttf, pixel_height);}
	if(data_font == 1) {	return lcd_mono_text_boxed(pozx, pozy, text, dum1_ttf, pixel_height);}
    return 0;
}
//=====================================================================================
int lcd_t_box(int pozx, int pozy, const char *text, int data_font, float pixel_height)
{
	if(data_font == 0) {	return lcd_text_boxed(pozx, pozy, text, digital_7_ttf, pixel_height);}
	if(data_font == 1) {	return lcd_text_boxed(pozx, pozy, text, dum1_ttf, pixel_height);}
    return 0;
}
//=====================================================================================
//===================================================================================

//===================================================================================

//===================================================================================

//===================================================================================

//===================================================================================

//===================================================================================

//===================================================================================

//===================================================================================

//===================================================================================



#endif /* TTF_H_ */
