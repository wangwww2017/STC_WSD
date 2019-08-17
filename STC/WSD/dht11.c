#include "dht11.h"

#include <intrins.h>
#include "lcd_086.h"

extern uint humi_dat,temp_dat;
uchar data_byte;
uchar Tem_Z,Tem_X,Hum_Z,Hum_X,Check;

//文件名是dht11.c 

//请根据自己的dht11接的IO口来改动位定义
sbit DHT = P5^3; 
//防止在与硬件通信时发生死循环的计数范围 
#define NUMBER 50 
#define SIZE 5 
static unsigned char status; 
//存放五字节数据的数组
static unsigned char value_array[SIZE]; 
/*可在其他的文件引用温湿度值,实际是温度的整数的10倍
如dht11读回的温度是26,则temp_value = 260, 湿度同理*/ 
int temp_value, humi_value; 

extern void Delay_1ms(unsigned int ms) 
{ 
	unsigned int x, y; 
	for(x = ms; x > 0; x--) 
	{ 
		for(y = 400; y > 0; y--); 
	} 
} 
static void DHT11_Delay_10us(void) 
{ 
		unsigned char i; 
		for(i=0;i<10;i++){
	   		_nop_();
			_nop_();
	   }
} 

void DHT11_Delay_us(uint us)	//(us*9+16)us
{

   while(us--){
   		_nop_();
		_nop_();
   }
//   while(us--);
 //  while(us--);

}

/********主机发出开始信号*******/ 
void DHT11_Start()  
{
  DHT=1;
  DHT11_Delay_us(10);
  DHT=0;
  Delay_1ms(20);               //保持电平时间大于18ms,延时20ms
  DHT=1;
  DHT11_Delay_us(40);
  DHT=1;                  //拉高等待DHT11响应

}
//DHT11接收一个字节数据
uchar receive_data()
{
  uchar i,temp,count;
  data_byte=0;
  for(i=0;i<8;i++)
   {
   count=2;
   while((!DHT)&&count++);           //等待50us低电平结束
   DHT11_Delay_us(30);                      //等待40us高电平，后判断电平高低,高:DHT为1,低:DHT为0
    
   temp=0; 
   if(DHT==1)    temp=1;             //DHT为高:1,位为1
   count=2;
   while((DHT)&&count++);            //等待DHT高电平结束     
   if(count==1)      break;          //DHT为低:0,位为0
   data_byte<<=1;                    //字节左移
   data_byte|=temp;
   }  ST =0;
  return data_byte;                  //返回读取值
}

//DHT11接收数据
void Rec_DHT()
{
 uchar flag;
 uchar T_H,T_L,H_H,H_L,check,num_check;   //定义采集参数中间变量
  DHT11_Start();                          //主机发出开始信号
 if(!DHT)                                 //等待从机响应信号输出低电平
  {
  flag = 2;
  while((!DHT) && flag++);    //判断从机是否发出 80us 的低电平响应信号是否结束
  flag = 2;
  while(DHT && flag++);       //判断从机是否发出 80us 的高电平，若发出则开始采集数据
  H_H=receive_data();         //采集湿度整数部分
  H_L=receive_data();         //采集湿度小数部分
  T_H=receive_data();         //采集温度整数部分
  T_L=receive_data();         //采集温度小数部分
  check=receive_data();       //采集校验位
  DHT = 1;
  num_check=H_H+H_L+T_H+T_L;
 // if(num_check=check)         //进行校核
  // {
   humi_dat	= H_H*10; 
   Hum_X 	= H_L; 	
   temp_dat = T_H*10;
   Tem_X  	= T_L;
  // }
 }
}

/*****************************************************************************/ 


	/*
         //湿度整数部分
         str1[0] = (char)(0X30+RH_data/10); 
         str1[1] = (char)(0X30+RH_data%10);
         str1[2] =  0x2e; //小数点
          //湿度小数部分
         str1[3] = (char)(0X30+RL_data/10); 
          str1[5] = 0X25; //"%"
         str1[6] = 0X52; //"R"
         str1[7] = 0X48; //"H"
         //温度整数部分
         str2[0] = (char)(0X30+TH_data/10); 
         str2[1] = (char)(0X30+TH_data%10);
         str2[2] =  0x2e; //小数点
         //温度小数部分
          str2[3] = (char)(0X30+TL_data/10); 
          str2[5] = 0X27;  //"'"
          str2[6] = 0X43;  //"C"
		  */
