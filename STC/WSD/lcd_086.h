#ifndef _LCD_086_H
#define _LCD_086_H

#include "global.h"

extern void initial_lcd();
extern void clear_screen();
extern void display_128x64(uchar *dp);
extern void display_graphic_16x16(uint page,uint column,uchar *dp);
extern void display_graphic_8x16(uint page,uchar column,uchar *dp);
extern void display_graphic_5x7(uint page,uchar column,uchar *dp);
extern void display_GB2312_string(uchar y,uchar x,uchar *text);
extern void display_string_5x7(uchar y,uchar x,uchar *text);
extern void lcd_delay(int n_ms);


#endif

