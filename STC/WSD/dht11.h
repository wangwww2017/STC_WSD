#ifndef _DHT11_H_
#define _DHT11_H_

#include "global.h"

sbit ST = P5^4;

extern void Delay_1ms(unsigned int ms); 
//�����ķ���ֵ��ʾ��ȡ�����Ƿ�ɹ�OK��ʾ�ɹ�ERROR��ʾʧ��
extern void Rec_DHT();

#endif


