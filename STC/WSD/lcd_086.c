#include "lcd_086.h"
//#include <../STC12C5A60S2.h>



sbit lcd_sclk=P2^4;  /*接口定义:lcd_sclk就是LCD的sclk*/
sbit lcd_sid=P5^0;   /*接口定义:lcd_sid就是LCD的sid*/
sbit lcd_rs=P5^1;    /*接口定义:lcd_rs就是LCD的rs*/
sbit lcd_reset=P2^5; /*接口定义:lcd_reset就是LCD的reset*/
sbit lcd_cs1=P2^6;   /*接口定义:lcd_cs1就是LCD的cs1*/
//sbit lcd_led=P2^3;
sbit Rom_IN=P4^0;    /*字库IC接口定义:Rom_IN就是字库IC的SI*/
sbit Rom_OUT=P2^0;   /*字库IC接口定义:Rom_OUT就是字库IC的SO*/
sbit Rom_SCK=P2^1;   /*字库IC接口定义:Rom_SCK就是字库IC的SCK*/
sbit Rom_CS=P2^2;    /*字库IC接口定义Rom_CS就是字库IC的CS#*/

uchar code bmp1[];

uchar code jiong1[]={/*--  文字:  囧  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0xFE,0x82,0x42,0xA2,0x9E,0x8A,0x82,0x86,0x8A,0xB2,0x62,0x02,0xFE,0x00,0x00,
0x00,0x7F,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x40,0x7F,0x00,0x00};

uchar code lei1[]={/*--  文字:  畾  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x80,0x80,0x80,0xBF,0xA5,0xA5,0xA5,0x3F,0xA5,0xA5,0xA5,0xBF,0x80,0x80,0x80,0x00,
0x7F,0x24,0x24,0x3F,0x24,0x24,0x7F,0x00,0x7F,0x24,0x24,0x3F,0x24,0x24,0x7F,0x00};


/*写指令到LCD模块*/
void transfer_command_lcd(int data1)   
{
	char i;
	lcd_rs=0;
	for(i=0;i<8;i++)
   {
		lcd_sclk=0;
		if(data1&0x80) lcd_sid=1;
		else lcd_sid=0;
		lcd_sclk=1;
	 	data1=data1<<=1;
   }
}

/*写数据到LCD模块*/
void transfer_data_lcd(int data1)
{
	char i;
	lcd_rs=1;
	for(i=0;i<8;i++)
   {
		lcd_sclk=0;
		if(data1&0x80) lcd_sid=1;
		else lcd_sid=0;
		lcd_sclk=1;
	 	data1=data1<<=1;
   }
}

/*延时*/
void lcd_delay(int n_ms)               
{
 int j,k;
 for(j=0;j<n_ms;j++)
 for(k=0;k<110;k++);
}


/*等待一个按键，我的主板是用P2.0与GND之间接一个按键*/
/*void waitkey()
{
 repeat:
  	if (P2&0x01) goto repeat;
	else lcd_delay(6);
	if (P2&0x01) goto repeat;
	else
	lcd_delay(40);;
}
*/

/*LCD模块初始化*/
void initial_lcd()
{

	lcd_cs1=0;
	Rom_CS = 1;
	lcd_reset=0;        /*低电平复位*/
	lcd_delay(20);
	lcd_reset=1;		    /*复位完毕*/
	lcd_delay(20);        
	transfer_command_lcd(0xe2);	 /*软复位*/
	lcd_delay(5);
	transfer_command_lcd(0x2c);  /*升压步聚1*/
	lcd_delay(5);	
	transfer_command_lcd(0x2e);  /*升压步聚2*/
	lcd_delay(5);
	transfer_command_lcd(0x2f);  /*升压步聚3*/
	lcd_delay(5);
	transfer_command_lcd(0x23);  /*粗调对比度，可设置范围0x20～0x27*/
	transfer_command_lcd(0x81);  /*微调对比度*/
	transfer_command_lcd(0x2f);  /*0x28,微调对比度的值，可设置范围0x00～0x3f*/
	transfer_command_lcd(0xa2);  /*1/9偏压比（bias）*/
	transfer_command_lcd(0xc8);  /*行扫描顺序：从上到下*/
	transfer_command_lcd(0xa0);  /*列扫描顺序：从左到右*/
	transfer_command_lcd(0x40);  /*起始行：第一行开始*/
	transfer_command_lcd(0xaf);  /*开显示*/
	lcd_cs1=1;
}

void lcd_address(uint page,uint column)
{

	column=column-0x01;
	transfer_command_lcd(0xb0+page-1);   /*设置页地址*/
	transfer_command_lcd(0x10+(column>>4&0x0f));	/*设置列地址的高4位*/
	transfer_command_lcd(column&0x0f);	/*设置列地址的低4位*/	
}

/*全屏清屏*/
void clear_screen()
{
	unsigned char i,j;
	lcd_cs1=0;
	Rom_CS = 1;	
	for(i=0;i<9;i++)
	{
		transfer_command_lcd(0xb0+i);
		transfer_command_lcd(0x10);
		transfer_command_lcd(0x00);
		for(j=0;j<132;j++)
		{
		  	transfer_data_lcd(0x00);
		}
	}
 	lcd_cs1=1;
}

/*显示128x64点阵图像*/
void display_128x64(uchar *dp)
{
	uint i,j;
	lcd_cs1=0;
	for(j=0;j<8;j++)
	{
		lcd_address(j+1,1);
		for (i=0;i<128;i++)
		{	
			transfer_data_lcd(*dp);					/*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
			dp++;
		}
	}
	lcd_cs1=1;
}


/*显示16x16点阵图像、汉字、生僻字或16x16点阵的其他图标*/
void display_graphic_16x16(uint page,uint column,uchar *dp)
{
	uint i,j;
 	lcd_cs1=0;
	Rom_CS = 1; 	
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{	
			transfer_data_lcd(*dp);					/*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
			dp++;
		}
	}
	lcd_cs1=1;
}


/*显示8x16点阵图像、ASCII, 或8x16点阵的自造字符、其他图标*/
void display_graphic_8x16(uint page,uchar column,uchar *dp)
{
	uint i,j;
	lcd_cs1=0;	
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<8;i++)
		{	
			transfer_data_lcd(*dp);					/*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
			dp++;
		}
	}
	lcd_cs1=1;
}


/*显示5*7点阵图像、ASCII, 或5x7点阵的自造字符、其他图标*/
void display_graphic_5x7(uint page,uchar column,uchar *dp)
{
	uint col_cnt;
	uchar page_address;
	uchar column_address_L,column_address_H;
	page_address = 0xb0+page-1;
	
	lcd_cs1=0;	
	
	column_address_L =(column&0x0f)-1;
	column_address_H =((column>>4)&0x0f)+0x10;
	
	transfer_command_lcd(page_address); 		/*Set Page Address*/
	transfer_command_lcd(column_address_H);	/*Set MSB of column Address*/
	transfer_command_lcd(column_address_L);	/*Set LSB of column Address*/
	
	for (col_cnt=0;col_cnt<6;col_cnt++)
	{	
		transfer_data_lcd(*dp);
		dp++;
	}
	lcd_cs1=1;
}

/****送指令到晶联讯字库IC***/
void send_command_to_ROM( uchar datu )
{
	uchar i;
	for(i=0;i<8;i++ )
	{
		if(datu&0x80)
			Rom_IN = 1;
		else
			Rom_IN = 0;
			datu = datu<<1;
			Rom_SCK=0;
			Rom_SCK=1;
	}
}

/****从晶联讯字库IC中取汉字或字符数据（1个字节）***/
static uchar get_data_from_ROM( )
{
	
	uchar i;
	uchar ret_data=0;
	Rom_SCK=1;
	for(i=0;i<8;i++)
	{
		Rom_OUT=1;
		Rom_SCK=0;
		ret_data=ret_data<<1;
		if( Rom_OUT )
			ret_data=ret_data+1;
		else
			ret_data=ret_data+0;
		Rom_SCK=1;
	}
	return(ret_data);
}



/*从相关地址（addrHigh：地址高字节,addrMid：地址中字节,addrLow：地址低字节）中连续读出DataLen个字节的数据到 pBuff的地址*/
/*连续读取*/
void get_n_bytes_data_from_ROM(uchar addrHigh,uchar addrMid,uchar addrLow,uchar *pBuff,uchar DataLen )
{
	uchar i;
	Rom_CS = 0;
	lcd_cs1=1;	
	Rom_SCK=0;
	send_command_to_ROM(0x03);
	send_command_to_ROM(addrHigh);
	send_command_to_ROM(addrMid);
	send_command_to_ROM(addrLow);
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();
	Rom_CS = 1;
}


/******************************************************************/

ulong  fontaddr=0;
void display_GB2312_string(uchar y,uchar x,uchar *text)
{
	uchar i= 0;
	uchar addrHigh,addrMid,addrLow ;
	uchar fontbuf[32];			
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{						
			/*国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0*/
			/*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
			fontaddr = (text[i]- 0xb0)*94; 
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (ulong)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
			addrLow = fontaddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=2;
			x+=16;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{						
			/*国标简体（GB2312）15x16点的字符在晶联讯字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0*/
			/*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
			fontaddr = (text[i]- 0xa1)*94; 
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (ulong)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
			addrLow = fontaddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=2;
			x+=16;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{						
			unsigned char fontbuf[16];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16 );/*取16个字节的数据，存到"fontbuf[32]"*/
			
			display_graphic_8x16(y,x,fontbuf);/*显示8x16的ASCII字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=1;
			x+=8;
		}
		else
			i++;	
	}
	
}

void display_string_5x7(uchar y,uchar x,uchar *text)
{
	unsigned char i= 0;
	unsigned char addrHigh,addrMid,addrLow ;
	while((text[i]>0x00))
	{
		
		if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{						
			unsigned char fontbuf[8];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*8);
			fontaddr = (unsigned long)(fontaddr+0x3bfc0);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,8);/*取8个字节的数据，存到"fontbuf[32]"*/
			
			display_graphic_5x7(y,x,fontbuf);/*显示5x7的ASCII字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=1;
			x+=6;
		}
		else
		i++;	
	}
	
}

