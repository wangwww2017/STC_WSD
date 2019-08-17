#include "dht11.h"

#include <intrins.h>
#include "lcd_086.h"

extern uint humi_dat,temp_dat;
uchar data_byte;
uchar Tem_Z,Tem_X,Hum_Z,Hum_X,Check;

//�ļ�����dht11.c 

//������Լ���dht11�ӵ�IO�����Ķ�λ����
sbit DHT = P5^3; 
//��ֹ����Ӳ��ͨ��ʱ������ѭ���ļ�����Χ 
#define NUMBER 50 
#define SIZE 5 
static unsigned char status; 
//������ֽ����ݵ�����
static unsigned char value_array[SIZE]; 
/*�����������ļ�������ʪ��ֵ,ʵ�����¶ȵ�������10��
��dht11���ص��¶���26,��temp_value = 260, ʪ��ͬ��*/ 
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

/********����������ʼ�ź�*******/ 
void DHT11_Start()  
{
  DHT=1;
  DHT11_Delay_us(10);
  DHT=0;
  Delay_1ms(20);               //���ֵ�ƽʱ�����18ms,��ʱ20ms
  DHT=1;
  DHT11_Delay_us(40);
  DHT=1;                  //���ߵȴ�DHT11��Ӧ

}
//DHT11����һ���ֽ�����
uchar receive_data()
{
  uchar i,temp,count;
  data_byte=0;
  for(i=0;i<8;i++)
   {
   count=2;
   while((!DHT)&&count++);           //�ȴ�50us�͵�ƽ����
   DHT11_Delay_us(30);                      //�ȴ�40us�ߵ�ƽ�����жϵ�ƽ�ߵ�,��:DHTΪ1,��:DHTΪ0
    
   temp=0; 
   if(DHT==1)    temp=1;             //DHTΪ��:1,λΪ1
   count=2;
   while((DHT)&&count++);            //�ȴ�DHT�ߵ�ƽ����     
   if(count==1)      break;          //DHTΪ��:0,λΪ0
   data_byte<<=1;                    //�ֽ�����
   data_byte|=temp;
   }  ST =0;
  return data_byte;                  //���ض�ȡֵ
}

//DHT11��������
void Rec_DHT()
{
 uchar flag;
 uchar T_H,T_L,H_H,H_L,check,num_check;   //����ɼ������м����
  DHT11_Start();                          //����������ʼ�ź�
 if(!DHT)                                 //�ȴ��ӻ���Ӧ�ź�����͵�ƽ
  {
  flag = 2;
  while((!DHT) && flag++);    //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
  flag = 2;
  while(DHT && flag++);       //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ����������ʼ�ɼ�����
  H_H=receive_data();         //�ɼ�ʪ����������
  H_L=receive_data();         //�ɼ�ʪ��С������
  T_H=receive_data();         //�ɼ��¶���������
  T_L=receive_data();         //�ɼ��¶�С������
  check=receive_data();       //�ɼ�У��λ
  DHT = 1;
  num_check=H_H+H_L+T_H+T_L;
 // if(num_check=check)         //����У��
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
         //ʪ����������
         str1[0] = (char)(0X30+RH_data/10); 
         str1[1] = (char)(0X30+RH_data%10);
         str1[2] =  0x2e; //С����
          //ʪ��С������
         str1[3] = (char)(0X30+RL_data/10); 
          str1[5] = 0X25; //"%"
         str1[6] = 0X52; //"R"
         str1[7] = 0X48; //"H"
         //�¶���������
         str2[0] = (char)(0X30+TH_data/10); 
         str2[1] = (char)(0X30+TH_data%10);
         str2[2] =  0x2e; //С����
         //�¶�С������
          str2[3] = (char)(0X30+TL_data/10); 
          str2[5] = 0X27;  //"'"
          str2[6] = 0X43;  //"C"
		  */
