//#include <../STC12C5A60S2.h>
#include "lcd_086.h"
#include "dht11.h"
#include "global.h"
#include "ds1820.h"
#include "eeprom.h"
//unsigned char num[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x39};

#define ADDR_HR_H 	0x0000
#define ADDR_HR_L 	0x0001
#define ADDR_T_H 	0x0002
#define ADDR_T_L 	0x0003


sbit led = P0^4;
sbit beep = P1^0;

sbit key_up = P0^3;
sbit key_down = P0^2;
sbit key_ok = P0^1;
sbit key_cancel = P0^0;

sbit Port_humiH = P1^5;
sbit Port_humiL = P1^6;
sbit Port_tempH = P1^7;
sbit Port_tempL = P3^2;


int temp_val = 0;
int humi_dat,temp_dat;
int humi_setH,temp_setH;
int humi_setL,temp_setL;
int humi_alarmH,temp_alarmH;
int humi_alarmL,temp_alarmL;

uchar t_flag = 0;
uchar dht_flag = 0;

uchar KeyValue = 0;

unsigned char code cursor[] =                  // ���ݱ�
{
      0xF8,0xFC,0x84,0x84,0x04,0x02,0x02,0x07,
      0x05,0xD7,0xD4,0xD4,0x34,0x14,0x14,0x0C,
      0x00,0x00,0x00,0x00,0x01,0x03,0x02,0x02,
      0x02,0x02,0x02,0x03,0x00,0x00,0x00,0x00
};

uchar func_index = 0;
void (*current_operation_index)();

void display_main1();
void display_main2();
void display_wsd();
void display_set();			//�趨��ʪ��������1
void display_set2();		//�趨��ʪ��������2
void display_showSet();		//��ʾ����ֵ
void display_setSDH1();		//�趨ʪ������
void display_setWDH1();		//�趨�¶�����
void display_setSDL1();		//�趨ʪ������
void display_setWDL1();		//�趨�¶�����
void display_setSDH2();		//�趨ʪ������
void display_setWDH2();		//�趨�¶�����
void display_setSDL2();		//�趨ʪ������
void display_setWDL2();		//�趨�¶�����

typedef struct{
	uchar current;			//��ǰ������
	uchar up;				//�Ϸ�������
	uchar down;				//�·�������
	uchar ok;				//ȷ��������
	uchar cancel;           //����������
	void (*current_operation)(); 
}key_table;

key_table code table[14]={

	{0,0,1,2,0,(*display_main1)},
	{1,0,1,3,0,(*display_main2)},

	{2,2,2,13,0,(*display_wsd)},

	{3,3,4,5,0,(*display_set)},
	{4,3,4,7,0,(*display_set2)},
	
	{5,5,6,9,3,(*display_setSDH1)},
	{6,5,6,10,3,(*display_setWDH1)},
	{7,7,8,11,4,(*display_setSDL1)},
	{8,7,8,12,4,(*display_setWDL1)},

	{9,9,9,9,5,(*display_setSDH2)},
	{10,10,10,10,6,(*display_setWDH2)},	
	{11,11,11,11,7,(*display_setSDL2)},
	{12,12,12,12,8,(*display_setWDL2)},

	{13,13,13,3,2,(*display_showSet)},

};

void IO_Init();
void display_value(int val,unsigned char x,unsigned char y,unsigned char type);
void Delay_ms(unsigned int ms); 
uchar Key_Scan();
void Beep(uint ms);
void Led(uint ms);

void read_AlarmValue();
void write_AlarmValue();
//void Alarm();


void main(){

	Delay_ms(200);
	
	
	IO_Init();
	
	led = 0;beep = 0;
	
	initial_lcd();
	clear_screen();
	
	read_AlarmValue();

	while(1){
		
	   KeyValue = Key_Scan();
		//display_wsd();
		current_operation_index=table[func_index].current_operation;
			(*current_operation_index)();//ִ�е�ǰ��������	
	}

}





void Delay_ms(unsigned int ms) 
{ 
	unsigned int x, y; 
	for(x = ms; x > 0; x--) 
	{ 
		for(y = 400; y > 0; y--); 
	} 
} 

void Beep(uint ms){
   
	beep = 1;
	Delay_ms(ms);
	beep = 0;
}

void Led(uint ms){

	led = ~led;
}

void IO_Init(){
	P1M1 |= 0x00;
	P1M0 |= 0x01;
}



void read_AlarmValue(){
	
	Open_IAP();
	
	humi_alarmH	= Read_IAP_Byte(ADDR_HR_H);
	humi_alarmL	= Read_IAP_Byte(ADDR_HR_L);
    temp_alarmH	= Read_IAP_Byte(ADDR_T_H);
    temp_alarmL	= Read_IAP_Byte(ADDR_T_L);

	if(humi_alarmH == 0xff)
		humi_alarmH = 40;
	if(humi_alarmL == 0xff)
		humi_alarmH = 40;
	if(temp_alarmH == 0xff)
		temp_alarmH = 40;
	if(humi_alarmL == 0xff)
		temp_alarmL = 40;

   	Close_IAP();


}
void write_AlarmValue(){

	Open_IAP();
	
	Erase_IAP_Sector(ADDR_HR_H);
	Write_IAP_Byte(ADDR_HR_H,humi_alarmH);
	Write_IAP_Byte(ADDR_HR_L,humi_alarmL);
	Write_IAP_Byte(ADDR_T_H,temp_alarmH);
	Write_IAP_Byte(ADDR_T_L,temp_alarmL);

   	Close_IAP();
}
/*
void Alarm(){

	
}
*/

uchar Key_Scan(){
	
	uchar keycode = 0;

	if((key_up==0)||(key_down==0)||(key_ok==0)||(key_cancel==0))
	{
		Delay_ms(10);//����
		if(key_up==0)
		{
			
			func_index=table[func_index].up;    //���Ϸ�
			while(!key_up);//���ּ��
			keycode = KEY_UP;
		}
		if(key_down==0)
		{
		
			func_index=table[func_index].down;    //���·�
			while(!key_down);
			keycode = KEY_DOWN;
		}
		if(key_ok==0)
		{
		
			func_index=table[func_index].ok;    //ȷ��
			while(!key_ok);
			keycode = KEY_OK;
		}
		if(key_cancel==0)
		{
			func_index=table[func_index].cancel;    //ȡ��
			while(!key_cancel);
			keycode = KEY_CANCEL;
		}
		Beep(100);
		Led(20);
		clear_screen();
		return keycode;
	}				
	return keycode;
}


void display_main1(){
		
		display_GB2312_string(1,16,"��ʪ�Ȳ�����");	
		display_graphic_16x16(3,1,cursor);
		display_GB2312_string(3,22,"������ʪ��");	
		display_GB2312_string(5,22,"����ֵ�趨");
		display_GB2312_string(7,72,"Ver 1.0");
}


void display_main2(){
	
		display_GB2312_string(1,16,"��ʪ�Ȳ�����");	
		display_graphic_16x16(5,1,cursor);
		display_GB2312_string(3,22,"������ʪ��");	
		display_GB2312_string(5,22,"����ֵ�趨");
		display_GB2312_string(7,72,"Ver 1.0");
}

void display_wsd(){

		display_GB2312_string(1,16,"��ʪ�Ȳ�����");	
		display_GB2312_string(3,1,"ʪ��:");	
		display_GB2312_string(5,1,"�¶�1:");
		display_GB2312_string(7,1,"�¶�2:");
	
		temp_val = ReadOne_T();
		display_value(temp_val,7,48,1);
	
		Rec_DHT();
		display_value(humi_dat,3,48,2);
		display_value(temp_dat,5,48,1);

		//�������
		//�ж�ʪ��
		if( humi_dat < humi_alarmL*10){
			Beep(1000);
			Port_humiL = 0;
			display_GB2312_string(3,104,"��");
		}else
		 if( humi_dat> humi_alarmH*10){
			Beep(1000);
			Port_humiH = 0;
			display_GB2312_string(3,104,"��");
					 
		}else
		{
			Port_humiH = 1;
			
			display_GB2312_string(3,104,"��");
		}
		
		//�ж��¶�
		if( temp_val< temp_alarmL*10){
			Beep(1000);
			Port_tempL = 0;
			display_GB2312_string(7,104,"��");
		}else
		 if( temp_val> temp_alarmH*10){
			Beep(1000);
			Port_tempH = 0;
			display_GB2312_string(7,104,"��");
		}else
		{
			Port_tempH = 1;
			
			display_GB2312_string(7,104,"��");
		}
}


void display_set(){
		
		display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(3,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȸ�");	
		display_GB2312_string(3,82,"�¶ȸ�");
		display_GB2312_string(5,18,"ʪ�ȵ�");	
		display_GB2312_string(5,82,"�¶ȵ�");
		display_GB2312_string(7,1,"ѡ��ֵ");
		display_GB2312_string(7,72,"Ver 1.0");
	
		humi_setH = humi_alarmH;humi_setL = humi_alarmL;
		temp_setH = temp_alarmH;temp_setL = temp_alarmL;

}

void display_set2(){


		display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(5,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȸ�");	
		display_GB2312_string(3,82,"�¶ȸ�");
		display_GB2312_string(5,18,"ʪ�ȵ�");	
		display_GB2312_string(5,82,"�¶ȵ�");
		display_GB2312_string(7,1,"ѡ��ֵ");
		display_GB2312_string(7,72,"Ver 1.0");
	
		humi_setH = humi_alarmH;humi_setL = humi_alarmL;
		temp_setH = temp_alarmH;temp_setL = temp_alarmL;
		
}


void display_showSet(){

	
		display_GB2312_string(1,16,"��ʪ�ȱ���ֵ");	
		display_GB2312_string(3,22,"��");
		display_GB2312_string(3,91,"��");	
	
	
		
		display_value(humi_alarmH*10,5,4,2);
		display_value(humi_alarmL*10,5,74,2);
		display_value(temp_alarmH*10,7,4,1);
		display_value(temp_alarmL*10,7,74,1);
}


//�趨 ʪ�ȸ���
void display_setSDH1(){
	
	    display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(3,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȸ�");	
		display_GB2312_string(5,18,"�¶ȸ�");
		display_GB2312_string(7,1,"ѡ��ʪ��");
		display_GB2312_string(7,72,"Ver 1.0");

		humi_setH = humi_alarmH;
		temp_setH = temp_alarmH;

		display_value(humi_setH*10,3,65,2);
		display_value(temp_setH*10,5,65,1);

}

//�趨�¶�����		
void display_setWDH1(){

	 	display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(5,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȸ�");	
		display_GB2312_string(5,18,"�¶ȸ�");
		display_GB2312_string(7,1,"ѡ���¶�");
		display_GB2312_string(7,72,"Ver 1.0");

		humi_setH = humi_alarmH;
		temp_setH = temp_alarmH;

		display_value(humi_setH*10,3,65,2);
		display_value(temp_setH*10,5,65,1);

}

//�趨 ʪ�ȸ���
void display_setSDH2(){
	
	    display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(3,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȸ�");	
		display_GB2312_string(5,18,"�¶ȸ�");
		display_GB2312_string(7,1,"ʪ������");
		display_GB2312_string(7,72,"Ver 1.0");

		
	 	switch(KeyValue){
			
			case KEY_UP: humi_setH = humi_setH+1;
				if(humi_setH>80)humi_setH = 20;
				display_GB2312_string(7,1,"�趨��");
				break;
			case KEY_DOWN: humi_setH = humi_setH-1;
				if(humi_setH<20)humi_setH = 80;
				display_GB2312_string(7,1,"�趨��");
				break;
			case KEY_OK: humi_alarmH = humi_setH;write_AlarmValue();
				display_GB2312_string(7,1,"�趨���");
				break;
			 default:
			 break;
		}
		

		display_value(humi_setH*10,3,65,2);
		display_value(temp_setH*10,5,65,1);

}


//�趨�¶�����		
void display_setWDH2(){

	 	display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(5,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȸ�");	
		display_GB2312_string(5,18,"�¶ȸ�");
		display_GB2312_string(7,1,"�¶�����");
		display_GB2312_string(7,72,"Ver 1.0");

	 	switch(KeyValue){
			
			case KEY_UP: temp_setH = temp_setH+1;
				if(temp_setH>125)temp_setH = -55;
				display_GB2312_string(7,1,"�趨��");
				break;
			case KEY_DOWN: temp_setH = temp_setH-1;
				if(temp_setH<-55)temp_setH = 125;
				display_GB2312_string(7,1,"�趨��");
				break;
			case KEY_OK: temp_alarmH = temp_setH;write_AlarmValue();
				display_GB2312_string(7,1,"�趨���");
				break;
			 default:
			 break;
		}
		

		display_value(humi_setH*10,3,65,2);
		display_value(temp_setH*10,5,65,1);

}


//�趨 ʪ�� ����
void display_setSDL1(){
	
	    display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(3,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȵ�");	
		display_GB2312_string(5,18,"�¶ȵ�");
		display_GB2312_string(7,1,"ѡ��ʪ��");
		display_GB2312_string(7,72,"Ver 1.0");
		
		humi_setL = humi_alarmL;
		temp_setL = temp_alarmL;

		display_value(humi_setL*10,3,65,2);
		display_value(temp_setL*10,5,65,1);

}

//�趨�¶�����		
void display_setWDL1(){

	 	display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(5,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȵ�");	
		display_GB2312_string(5,18,"�¶ȵ�");
		display_GB2312_string(7,1,"ѡ���¶�");
		display_GB2312_string(7,72,"Ver 1.0");

		humi_setL = humi_alarmL;
		temp_setL = temp_alarmL;

		display_value(humi_setL*10,3,65,2);
		display_value(temp_setL*10,5,65,1);

}


//�趨 ʪ�� ����
void display_setSDL2(){
	
	    display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(3,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȵ�");	
		display_GB2312_string(5,18,"�¶ȵ�");
		display_GB2312_string(7,1,"ʪ������");
		display_GB2312_string(7,72,"Ver 1.0");

		
	 	switch(KeyValue){
			
			case KEY_UP: humi_setL = humi_setL+1;
				if(humi_setL>80)humi_setL = 20;
				display_GB2312_string(7,1,"�趨��");
				break;
			case KEY_DOWN: humi_setL = humi_setL-1;
				if(humi_setL<20)humi_setL = 80;
				display_GB2312_string(7,1,"�趨��");
				break;
			case KEY_OK: humi_alarmL = humi_setL;write_AlarmValue();
				display_GB2312_string(7,1,"�趨���");
				break;
			 default:
			 break;
		}
		

		display_value(humi_setL*10,3,65,2);
		display_value(temp_setL*10,5,65,1);

}


//�趨�¶�����		
void display_setWDL2(){

	 	display_GB2312_string(1,16,"�趨��ʪ��ֵ");	
		display_graphic_16x16(5,1,cursor);
		display_GB2312_string(3,18,"ʪ�ȵ�");	
		display_GB2312_string(5,18,"�¶ȵ�");
		display_GB2312_string(7,1,"�¶�����");
		display_GB2312_string(7,72,"Ver 1.0");

	 	switch(KeyValue){
			
			case KEY_UP: temp_setL = temp_setL+1;
				if(temp_setL>125)temp_setL = -55;
				display_GB2312_string(7,1,"�趨��");
				break;
			case KEY_DOWN: temp_setL = temp_setL-1;
				if(temp_setL<-55)temp_setL = 125;
				display_GB2312_string(7,1,"�趨��");
				break;
			case KEY_OK: temp_alarmL = temp_setL;write_AlarmValue();
				display_GB2312_string(7,1,"�趨���");
				break;
			 default:
			 break;
		}
		

		display_value(humi_setL*10,3,65,2);
		display_value(temp_setL*10,5,65,1);

}


void display_value( int val,unsigned char y,unsigned char x,unsigned char type){
	
	unsigned char tmp[5]={'0','0','0','.','0'};
	if(t_flag || val<0)
	{
		tmp[0] = '-';
		if(val <0 )
		val = val*(-1); 
	}
	else
	{
		tmp[0] = val/1000+0x30;
	}
	t_flag = 0;
	tmp[1] = val%1000/100+0x30;
	tmp[2] = val%1000%100/10+0x30;
	tmp[4] = val%100%100%10+0x30;
	display_GB2312_string(y,x,tmp);
	switch(type){
		case 1:display_GB2312_string(y,x+40,"��");
			break;
		case 2:display_GB2312_string(y,x+40,"%");
			break;
	}
	
}

