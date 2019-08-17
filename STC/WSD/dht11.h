#ifndef _DHT11_H_
#define _DHT11_H_

#include "global.h"

sbit ST = P5^4;

extern void Delay_1ms(unsigned int ms); 
//函数的返回值表示读取数据是否成功OK表示成功ERROR表示失败
extern void Rec_DHT();

#endif


