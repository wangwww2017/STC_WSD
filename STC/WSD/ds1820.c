
#include "ds1820.h"


sbit DQ=P5^2;

extern uchar t_flag;

void delay_us(uint us)	//(us*9+16)us
{

   while(us--){
   		_nop_();
		_nop_();
   }
//   while(us--);
 //  while(us--);

}

//18b20重启
uchar Reset_18b20() 
{
	uchar count=0;
	DQ=1;
	DQ=0;
	delay_us(500);  //(55*9+16）=us
	DQ=1;
	delay_us(60);   //等待DS18B20的低电平 25us
	while(DQ)
	{
		count++;
		if(count==60)
			return 0;
	}
	delay_us(420);
	return 1;
/*
	DQ = 1;
	delay_us(55);
	DQ = 0;
	delay_us(500);
	DQ = 1;
	delay_us(60);
	count = DQ;
	return count;
*/
}

//单总线写一字节
void WriteByte(uchar dat)
{
	uchar i=0,w;
	for(i=0;i<8;i++)
	{
	  w=dat&0x01;
	  dat=dat>>1;
		if(w)	   //判断是写1还是写0
		{
			DQ=0;
			//延时15us;
		  delay_us(1);
			//写1
			DQ=1;									
			delay_us(60);		;
		}

		else
		{
			DQ=0;
			 delay_us(1);
			//写0
			DQ=0;
		    delay_us(60);		
		}
	  DQ=1;	  //每传送一位后，需高最少1US
	  delay_us(1);
		
	 }
}

//从单总线读一字节
uchar ReadByte()
{
	 uchar tempdat=0,i=0;
	 DQ=1; //先拉高 
	 for(i=0;i<8;i++)
	 {	
	    tempdat=tempdat>>1;	
	 	DQ=0;
		 delay_us(1);
		DQ=1;
		 delay_us(1);	 
		/* */
		if(DQ==1)
			tempdat =tempdat|0x80;
		else  
			tempdat =tempdat|0x00;
		delay_us(60);  

	   DQ=1;	  //每传送一位后，拉高最少1US
	   delay_us(1);
	 }
	 return tempdat;
}


//读取单个温度
float ReadOne_T()
{
	int t;
	float t1;
	uchar tempH=0;
	uchar tempL=0;	
	if(!Reset_18b20()) 	//
		return 13;

	Reset_18b20();      //复位	
	WriteByte(0xcc);	//跳过ROM指令	
	WriteByte(0x44); 	//执行温度转换存储器操作指令
	delay_us(1000);		//等待>500us温度转换时间
	
	Reset_18b20();		//复位
	WriteByte(0xcc);	//跳过ROM指令	
	WriteByte(0xbe); 	//执行读RAM的存储器操作指令
	  
	tempL=ReadByte();	//读数据--温度的低4位字节
	tempH=ReadByte(); 	//读数据--温度的高4位字节
	//将读取温度转换成数字
	//t=tempH*256+tempL;
	t=tempH;
	t=t<<8;
	t=t|tempL;
	if(t<0x800)
	{
		t_flag = 0;
	}
	else if(t>=0x800)
	{
		t_flag=1;
		t=~t+1;
	}

	t1=t*0.625+0.5;
	return t1;
} 




